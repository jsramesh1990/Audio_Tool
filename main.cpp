
// Single-file small audio processor: reads WAV, applies pitch shift via resampling and biquad filters

#include <sndfile.h>
#include <vector>
#include <cmath>
#include <cstring>
#include <iostream>
#include <getopt.h>
#include <algorithm>

// Basic Biquad filter implementation
class Biquad {
public:
    enum Type { LOWPASS, HIGHPASS, PEAK, LOWSHELF };
    Biquad() { reset(); }
    void reset() { x1 = x2 = y1 = y2 = 0.0; }
    void setup(Type type, double fs, double f0, double dbGain, double Q) {
        double A = pow(10.0, dbGain/40.0);
        double omega = 2.0*M_PI*f0/fs;
        double sn = sin(omega);
        double cs = cos(omega);
        double alpha = sn/(2.0*Q);
        double b0,b1,b2,a0,a1,a2;
        switch(type){
            case LOWSHELF: {
                double sqrtA = sqrt(A);
                double beta = sqrtA/ Q;
                b0 =    A*( (A+1) - (A-1)*cs + 2*sqrtA*alpha );
                b1 =  2*A*( (A-1) - (A+1)*cs );
                b2 =    A*( (A+1) - (A-1)*cs - 2*sqrtA*alpha );
                a0 =        (A+1) + (A-1)*cs + 2*sqrtA*alpha;
                a1 =   -2*( (A-1) + (A+1)*cs );
                a2 =        (A+1) + (A-1)*cs - 2*sqrtA*alpha;
            } break;
            case PEAK: {
                b0 = 1 + alpha*A;
                b1 = -2*cs;
                b2 = 1 - alpha*A;
                a0 = 1 + alpha/A;
                a1 = -2*cs;
                a2 = 1 - alpha/A;
            } break;
            case HIGHPASS: {
                b0 =  (1+cs)/2;
                b1 = -(1+cs);
                b2 =  (1+cs)/2;
                a0 =  1 + alpha;
                a1 = -2*cs;
                a2 =  1 - alpha;
            } break;
            default: // LOWPASS
                b0 = (1-cs)/2;
                b1 = 1-cs;
                b2 = (1-cs)/2;
                a0 = 1 + alpha;
                a1 = -2*cs;
                a2 = 1 - alpha;
        }
        // normalize
        this->b0 = b0/a0; this->b1 = b1/a0; this->b2 = b2/a0;
        this->a1 = a1/a0; this->a2 = a2/a0;
        reset();
    }
    double process(double x){
        double y = b0*x + b1*x1 + b2*x2 - a1*y1 - a2*y2;
        x2 = x1; x1 = x; y2 = y1; y1 = y;
        return y;
    }
private:
    double b0=1,b1=0,b2=0,a1=0,a2=0;
    double x1=0,x2=0,y1=0,y2=0;
};

// Simple linear resampler for pitch shift (changes length): resample by ratio
std::vector<float> resample_linear(const std::vector<float>& in, double ratio, int channels){
    if (ratio == 1.0) return in;
    size_t inFrames = in.size()/channels;
    size_t outFrames = (size_t)std::ceil(inFrames * ratio);
    std::vector<float> out(outFrames*channels);
    for(size_t f=0; f<outFrames; ++f){
        double inPos = f / ratio;
        size_t i0 = (size_t)floor(inPos);
        size_t i1 = std::min(i0+1, inFrames-1);
        double t = inPos - i0;
        for(int c=0;c<channels;++c){
            float s0 = in[(i0*channels)+c];
            float s1 = in[(i1*channels)+c];
            out[(f*channels)+c] = (float)((1.0 - t)*s0 + t*s1);
        }
    }
    return out;
}

void print_usage(const char* argv0){
    std::cout<<"Usage: "<<argv0<<" -i input.wav -o output.wav [--pitch semitones] [--lowshelf f gain_db Q] [--peak f gain_db Q] [--hp f Q] \n";
}

int main(int argc, char** argv){
    std::string inPath, outPath;
    double pitchSemis = 0.0;
    bool useLowShelf=false, usePeak=false, useHP=false;
    double ls_f=100, ls_gain=0, ls_q=0.7;
    double pk_f=1000, pk_gain=0, pk_q=1.0;
    double hp_f=20, hp_q=0.707;

    static struct option long_options[] = {
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {"pitch", required_argument, 0, 'p'},
        {"lowshelf", required_argument, 0, 0},
        {"peak", required_argument, 0, 0},
        {"hp", required_argument, 0, 0},
        {0,0,0,0}
    };
    int opt;
    int optidx=0;
    while((opt = getopt_long(argc, argv, "i:o:p:", long_options, &optidx))!=-1){
        if(opt=='i') inPath = optarg;
        else if(opt=='o') outPath = optarg;
        else if(opt=='p') pitchSemis = atof(optarg);
        else if(opt==0){
            std::string name = long_options[optidx].name;
            if(name=="lowshelf"){
                // expects 3 args: f gain Q
                if(optind+2 >= argc){ std::cerr<<"lowshelf requires 3 args\n"; return 1; }
                ls_f = atof(argv[optind]); ls_gain = atof(argv[optind+1]); ls_q = atof(argv[optind+2]);
                optind += 3; useLowShelf=true;
            } else if(name=="peak"){
                if(optind+2 >= argc){ std::cerr<<"peak requires 3 args\n"; return 1; }
                pk_f = atof(argv[optind]); pk_gain = atof(argv[optind+1]); pk_q = atof(argv[optind+2]);
                optind += 3; usePeak=true;
            } else if(name=="hp"){
                if(optind+1 >= argc){ std::cerr<<"hp requires 2 args\n"; return 1; }
                hp_f = atof(argv[optind]); hp_q = atof(argv[optind+1]);
                optind += 2; useHP=true;
            }
        }
    }
    if(inPath.empty() || outPath.empty()){ print_usage(argv[0]); return 1; }

    // Read input with libsndfile
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* inFile = sf_open(inPath.c_str(), SFM_READ, &sfinfo);
    if(!inFile){ std::cerr<<"Failed to open input: "<<sf_strerror(NULL)<<"\n"; return 1; }
    int channels = sfinfo.channels;
    int samplerate = sfinfo.samplerate;
    std::vector<float> data(sfinfo.frames * channels);
    sf_count_t readcount = sf_readf_float(inFile, data.data(), sfinfo.frames);
    sf_close(inFile);
    if(readcount != sfinfo.frames){ std::cerr<<"Warning: read fewer frames than expected\n"; }

    // Pitch shift by resampling: semitones -> ratio
    double ratio = pow(2.0, pitchSemis/12.0);
    // To pitch up by +n semitones while keeping original length, normally you need time-stretch + pitch shift.
    // This simple tool *changes* playback speed (length changes). For small demos that's acceptable.
    std::vector<float> resampled = resample_linear(data, ratio, channels);

    // Prepare filters per channel
    Biquad ls, pk, hp;
    if(useLowShelf) ls.setup(Biquad::LOWSHELF, samplerate*ratio, ls_f, ls_gain, ls_q);
    if(usePeak) pk.setup(Biquad::PEAK, samplerate*ratio, pk_f, pk_gain, pk_q);
    if(useHP) hp.setup(Biquad::HIGHPASS, samplerate*ratio, hp_f, 0.0, hp_q);

    // Process
    for(size_t frame=0; frame < resampled.size()/channels; ++frame){
        for(int c=0;c<channels;++c){
            double s = resampled[frame*channels + c];
            if(useHP) s = hp.process(s);
            if(useLowShelf) s = ls.process(s);
            if(usePeak) s = pk.process(s);
            // simple clipping
            if(s > 1.0) s = 1.0; if(s < -1.0) s = -1.0;
            resampled[frame*channels + c] = (float)s;
        }
    }

    // Write output
    SF_INFO outInfo = sfinfo;
    outInfo.frames = resampled.size()/channels;
    outInfo.samplerate = (int)std::llround(samplerate * ratio);
    SNDFILE* outFile = sf_open(outPath.c_str(), SFM_WRITE, &outInfo);
    if(!outFile){ std::cerr<<"Failed to open output for writing: "<<sf_strerror(NULL)<<"\n"; return 1; }
    sf_count_t written = sf_writef_float(outFile, resampled.data(), outInfo.frames);
    sf_close(outFile);
    std::cout<<"Wrote "<<written<<" frames to "<<outPath<<" (samplerate="<<outInfo.samplerate<<")\n";
    return 0;
}

