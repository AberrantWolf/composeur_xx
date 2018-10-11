#include <iostream>
#include <ctype.h>

#include "portaudio.h"

constexpr uint32_t SAMPLE_RATE = 44100;
constexpr uint32_t NUM_SECONDS = 1;
constexpr float AMPLITUDE = 0.5f;

typedef struct
{
	float left_phase;
	float right_phase;
}
paTestData;

/* This routine will be called by the PortAudio engine when audio is needed.
It may called at interrupt level on some machines so don't do anything
that could mess up the system like calling malloc() or free().
*/
static int patestCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	/* Cast data passed through stream to our structure. */
	paTestData *data = (paTestData*)userData;
	float *out = (float*)outputBuffer;
	unsigned int i;
	(void)inputBuffer; /* Prevent unused variable warning. */

	for (i = 0; i<framesPerBuffer; i++)
	{
		*out++ = data->left_phase * AMPLITUDE;  /* left */
		*out++ = data->right_phase * AMPLITUDE;  /* right */
									 /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
		data->left_phase += 0.01f;
		/* When signal reaches top, drop back down. */
		if (data->left_phase >= 1.0f) data->left_phase -= 2.0f;
		/* higher pitch so we can distinguish left and right. */
		data->right_phase += 0.03f;
		if (data->right_phase >= 1.0f) data->right_phase -= 2.0f;
	}
	return 0;
}

void main() {
	PaError err;
	
	err = Pa_Initialize();
	if (err != paNoError) {
		std::cout << "Error on initialize: " << Pa_GetErrorText(err) << std::endl;
		return;
	}
	
	static paTestData data;

	PaStream* stream;
	err = Pa_OpenDefaultStream(
		&stream,
		0,
		2,
		paFloat32,
		SAMPLE_RATE,
		256,
		patestCallback,
		&data);

	if (err != paNoError) {
		std::cout << "Error on open default stream: " << Pa_GetErrorText(err) << std::endl;
		return;
	}

	std::cout << "Hello..." << std::endl;

	err = Pa_StartStream(stream);
	if (err != paNoError) {
		std::cout << "Error on starting stream: " << Pa_GetErrorText(err) << std::endl;
		return;
	}

	Pa_Sleep(1000 * NUM_SECONDS);

	err = Pa_StopStream(stream);
	if (err != paNoError) {
		std::cout << "Error on stopping stream: " << Pa_GetErrorText(err) << std::endl;
		return;
	}

	std::cout << "...World" << std::endl;

	err = Pa_CloseStream(stream);
	if (err != paNoError) {
		std::cout << "Error on close stream: " << Pa_GetErrorText(err) << std::endl;
		return;
	}

	err = Pa_Terminate();
	if (err != paNoError) {
		std::cout << "Error on terminate: " << Pa_GetErrorText(err) << std::endl;
	}
}