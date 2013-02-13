/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;

static Metadata M = Metadata()
    .title("TinySynth")
    .package("com.sifteo.sdk.synth", "1.0")
    .icon(Icon)
    .cubeRange(1);

static const CubeID cube = 0;
static VideoBuffer vid;

const AudioChannel whiteNoiseChannel(1);

static int16_t whiteNoise[3000];
static const AssetAudio noiseAsset = AssetAudio::fromPCM(whiteNoise);

void synthInit()
{
    Random gen = Random();

    //Generate the White Noise
    for (int i = 0; i != arraysize(whiteNoise); i++) {
        whiteNoise[i] = gen.random() * 0x7fff;
    }

    whiteNoiseChannel.play(noiseAsset);
}

void synthesize(float hz, float timbre, float volume)
{
    LOG("hz=%f timbre=%f volume=%f\n", hz, timbre, volume);

    whiteNoiseChannel.setVolume(volume * 96.f);
    whiteNoiseChannel.setSpeed(hz * arraysize(whiteNoise));
}

void main()
{
    unsigned fg = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::BLUE;
    unsigned bg = BG0ROMDrawable::SOLID_FG ^ BG0ROMDrawable::BLACK;

    vid.initMode(BG0_ROM);
    vid.attach(cube);
    vid.bg0rom.erase(bg);
    vid.bg0rom.fill(vec(0,0), vec(3,3), fg);

    synthInit();

    float hz = 0;

    while (1) {
        // Scale to [-1, 1]
        auto accel = cube.accel() / 128.f;

        // Glide to the target note (half-steps above or below middle C)
        float note = 261.6f * pow(1.05946f, 8 + round(accel.y * 24.f));
        hz += (note - hz) * 0.4f;

        synthesize(hz, accel.x - 0.2f,
            clamp(accel.x + 0.5f, 0.f, 1.f));

        const Int2 center = LCD_center - vec(24,24)/2;
        vid.bg0rom.setPanning(-(center + accel.xy() * 60.f));
        System::paint();
    }
}
