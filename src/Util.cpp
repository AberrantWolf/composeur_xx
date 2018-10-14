#include "Util.h"

#include <iostream>

compx::util::Rate compx::util::SAMPLE_RATE = 44'100.0f;

auto compx::util::get_pitch_from_note(std::string note_string) -> float {
	constexpr char note_names[] = { 'A', '#', 'B', 'C', '#', 'D', '#', 'E', 'F', '#', 'G', '#' };
	constexpr size_t note_count = 12;
	constexpr float A4 = 440.0f;
	static const float C0 = A4 * std::pow(2.0f, -4.75f);

	if (note_string.size() < 1) {
		return -1;
	}

	auto note_name = note_string.at(0);
	int note = note_count;
	for (int i = 0; i< note_count; ++i)
	{
		if (note_name == note_names[i]) {
			note = i;
			break;
		}
	}

	if (note >= note_count) {
		return -1;
	}

	size_t idx = 1;
	while (idx < note_string.size()) {
		switch (note_string[idx]) {
		case '#':
			++note;
			break;
		case 'b':
			--note;
			break;
		default:
			// I hate breaking out of while loops from within a switch statement
			goto octave;
			break;
		}

		++idx;
	}


octave:
	note = note % note_count;
	auto octave_string = note_string.substr(idx, note_string.size() - idx);
	if (octave_string.size() < 1) {
		return -1;
	}

	auto octave = 4;

	try {
		octave = std::stoi(octave_string);
	}
	catch (std::invalid_argument ex) {
		std::cout << "Invalid argument for octave: " << ex.what() << std::endl;
	}

	if (note < 3) {
		note += 12 + ((octave - 1) * 12) + 1 - 49;
	}
	else {
		note += ((octave - 1) * 12) + 1 - 49;
	}

	return A4 * std::pow(2.0f, note / 12.0f);
}