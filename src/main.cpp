#include <iostream>
#include <ctype.h>

#include "portaudio.h"

#include "Effects/Envelope.h"
#include "Instrument.h"
#include "Oscillators/Sine.h"
#include "Util.h"

constexpr uint32_t SAMPLE_RATE = 44100;
constexpr uint32_t NUM_SECONDS = 1;
constexpr float AMPLITUDE = 0.9f;

volatile bool working = true;

typedef struct
{
	std::shared_ptr<compx::Instrument> instr;
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
	static size_t counter = 0;

	/* Cast data passed through stream to our structure. */
	paTestData *data = (paTestData*)userData;
	float *out = (float*)outputBuffer;
	unsigned int i;
	(void)inputBuffer; /* Prevent unused variable warning. */

	for (i = 0; i<framesPerBuffer; i++)
	{
		data->instr->tick(counter);
		*out++ = data->instr->value() * AMPLITUDE;
		*out++ = data->instr->value() * AMPLITUDE;
		++counter;
	}

	static bool need_change = true;
	if (counter >= 44100 && need_change) {
		data->instr->set_note("Eb4");
		data->instr->start();
		need_change = false;
	}

	static bool need_change2 = true;
	if (counter >= 88200 && need_change2) {
		data->instr->set_note("B4");
		data->instr->start();
		need_change2 = false;
	}

	static bool need_change3 = true;
	if (counter >= 132'300 && need_change3) {
		data->instr->set_note("C5");
		data->instr->start();
		need_change3 = false;
	}

	if (counter > 176'400) {
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

	// Test conversion of notes:
	std::cout << "A4: " << compx::util::get_pitch_from_note("A4") << std::endl;
	std::cout << "A3: " << compx::util::get_pitch_from_note("A3") << std::endl;
	std::cout << "A2: " << compx::util::get_pitch_from_note("A2") << std::endl;
	std::cout << "A1: " << compx::util::get_pitch_from_note("A1") << std::endl;
	std::cout << "A0: " << compx::util::get_pitch_from_note("A0") << std::endl;
	std::cout << "A-1: " << compx::util::get_pitch_from_note("A-1") << std::endl;
	std::cout << "A#4: " << compx::util::get_pitch_from_note("A#4") << std::endl;
	std::cout << "A##4: " << compx::util::get_pitch_from_note("A##4") << std::endl;
	std::cout << "A#b4: " << compx::util::get_pitch_from_note("A#b4") << std::endl;
	std::cout << "Ab#4: " << compx::util::get_pitch_from_note("Ab#4") << std::endl;
	std::cout << "C4: " << compx::util::get_pitch_from_note("C4") << std::endl;
	std::cout << "G2: " << compx::util::get_pitch_from_note("G2") << std::endl;
	std::cout << "F#6: " << compx::util::get_pitch_from_note("F#6") << std::endl;
	std::cout << "Db5: " << compx::util::get_pitch_from_note("Db5") << std::endl;
	
	static paTestData data;

	// Make an instrument
	auto instrument = std::make_shared<compx::Instrument>();
	data.instr = instrument;

	// set up sine generators
	auto core_sine = std::make_shared<compx::osc::Sine>();
	auto secondary_sine = std::make_shared<compx::osc::Sine>();
	auto tertiary_sine = std::make_shared<compx::osc::Sine>();
	auto quaternary_sine = std::make_shared<compx::osc::Sine>();

	instrument->add_tone(core_sine, 1.0f, 0.6f);
	instrument->add_tone(secondary_sine, 1.98f, 0.3f);
	instrument->add_tone(tertiary_sine, 3.04f, 0.2f);
	instrument->add_tone(quaternary_sine, 1.5f, 0.05f);

	instrument->set_note("C4");

	// set up envelope effects
	auto envelope = instrument->get_envelope();

	envelope->appendPhase({ 0.01f, 0.0f });
	envelope->appendPhase({ 0.05f, 1.0f });
	envelope->appendPhase({ 0.15f, 0.6f });
	envelope->appendPhase({ 0.35f, 0.6f });
	envelope->appendPhase({ 0.15f, 0.0f });
	envelope->setSustainPhase(999);
	instrument->start();

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

	std::cin.ignore(1);
}