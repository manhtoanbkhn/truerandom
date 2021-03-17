// an easy implementation for truely random algo base on atmospheric noise
// this implementation is applied for predicting the result of
// power 6/55 vietlott lottery as an funny experiment
// feel to free to donate me if you win the jackpot when use this impl LOL
#define VC_EXTRALEAN
#pragma comment(lib,"winmm.lib")
#include <Windows.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

const int sample_rate = 4 * 4096; // must be supported by microphone
const int sample_size = 4096; // must be a power of 2

const unsigned int buffer_size = 2 * sample_size / 8; // make buffers 1/8 the size of the total wave buffer
char* buffer = new char[buffer_size];

long true_random_number(const long low, const long high)
{
    int i;
    long sum = 0;
    for (i = 0; i < buffer_size; ++i)
    {
        sum += abs((int)buffer[i]) * i;
    }
    sum = sum % (high - low + 1) + low;
    return sum;
}

int main(int argc, char** argv) {
    // vietlott power 6/55 params
    const long low = 1;
    const long high = 55;
    const int num = 6;
    for (unsigned int i = 0; i < buffer_size; i++) buffer[i] = 0;

    // setting up for audio recorder
    WAVEFORMATEX wfx = {};
    wfx.wFormatTag = WAVE_FORMAT_PCM;    // PCM is standard
    wfx.nChannels = 1;                   // 1 channel (mono)
    wfx.nSamplesPerSec = sample_rate;    // sample_rate
    wfx.wBitsPerSample = 16;             // 16 bit samples
    wfx.nBlockAlign = wfx.wBitsPerSample * wfx.nChannels / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec * wfx.nChannels;
    wfx.cbSize = 0;
    HWAVEIN wi;                             // open recording device
    WAVEHDR* pCurrent = new WAVEHDR();      // initialize header empty
    pCurrent->dwFlags = 0;                  // clear the 'done' flag
    pCurrent->dwBytesRecorded = 0;          // tell it no bytes have been recorded
    pCurrent->lpData = buffer;             // give it a pointer to our buffer
    pCurrent->dwBufferLength = buffer_size; // tell it the size of that buffer in bytes
    waveInOpen(&wi, WAVE_MAPPER, &wfx, NULL, NULL, CALLBACK_NULL | WAVE_FORMAT_DIRECT);
    waveInStart(wi); // start recording
    waveInPrepareHeader(wi, pCurrent, sizeof(WAVEHDR)); // prepare header

    int ignore = 0;
    int cnt = 0;
    vector<long> res;
    cout << "Jackpot is: ";
    while (true) {
        do {
            waveInAddBuffer(wi, pCurrent, sizeof(WAVEHDR));
            this_thread::sleep_for(chrono::milliseconds(1));
        } while ((pCurrent->dwFlags & WHDR_DONE) == 0);
        pCurrent->dwFlags &= ~WHDR_DONE;
        ignore++;
        if (ignore < 10) {
            continue; 
        }

        long t = true_random_number(low, high);
        bool exist = false;
        for (int i = 0; i < res.size(); ++i)
        {
            if (res[i] == t)
            {
                exist = true;
                break;
            }
        }
        if (exist) continue;
        cnt++;
        cout << t << " ";
        res.push_back(t);
        if (cnt == num) break;
        else
            this_thread::sleep_for(chrono::seconds(5));
    }
    waveInUnprepareHeader(wi, pCurrent, sizeof(WAVEHDR));
    waveInStop(wi); // once the user hits escape, stop recording, and clean up
    waveInClose(wi);

    return 0;
}