#include <iostream>
#include <ctype.h>

#include "portaudio.h"

#include "Effects/Envelope.h"
#include "Oscillators/Sine.h"

constexpr uint32_t SAMPLE_RATE = 44100;
constexpr uint32_t NUM_SECONDS = 1;
constexpr float AMPLITUDE = 0.8f;

volatile bool working = true;

typedef struct
{
	compx::fx::Envelope left_env;
	compx::fx::Envelope right_env;
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
	static long counter = 0;

	/* Cast data passed through stream to our structure. */
	paTestData *data = (paTestData*)userData;
	float *out = (float*)outputBuffer;
	unsigned int i;
	(void)inputBuffer; /* Prevent unused variable warning. */

	for (i = 0; i<framesPerBuffer; i++)
	{
		data->left_env.tick();
		data->right_env.tick();
		*out++ = data->left_env.value() * AMPLITUDE;
		*out++ = data->right_env.value() * AMPLITUDE;
		++counter;
	}

	if (counter > 88200) {
		working = false;
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

	// set up sine generators
	auto left_sine = std::make_shared<compx::osc::Sine>();
	auto right_sine = std::make_shared<compx::osc::Sine>();
	left_sine->set_freq(261.626f);
	right_sine->set_freq(311.127f);

	// set up envelope effects
	data.left_env = compx::fx::Envelope();
	//data.left_env.set_base(1.0f);
	data.left_env.set_source(left_sine);
	data.left_env.appendPhase({ 0.00f, 0.0f });
	data.left_env.appendPhase({ 0.05f, 1.0f });
	data.left_env.appendPhase({ 0.15f, 0.6f });
	data.left_env.appendPhase({ 0.35f, 0.0f });
	data.left_env.setSustainPhase(999);
	data.left_env.start();
	//data.left_env.release();

	data.right_env = compx::fx::Envelope();
	//data.right_env.set_base(1.0f);
	data.right_env.set_source(right_sine);
	data.right_env.appendPhase({ 1.00f, 0.0f });
	data.right_env.appendPhase({ 0.05f, 1.0f });
	data.right_env.appendPhase({ 0.15f, 0.6f });
	data.right_env.appendPhase({ 0.35f, 0.0f });
	data.right_env.setSustainPhase(999);
	data.right_env.start();
	//data.right_env.release();

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

	//Pa_Sleep(1000 * NUM_SECONDS);
	while (working);

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