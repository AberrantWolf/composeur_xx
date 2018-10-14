#pragma once

#include <tuple>

#include "Effects/Envelope.h"
#include "Effects/Mixer.h"
#include "Interfaces/IControllable.h"
#include "Interfaces/IFrequencyGen.h"
#include "Interfaces/IGenerator.h"
#include "Util.h"

namespace compx {

// Instrument maintains a list of oscillators in order to adjust them to specific pitches,
// as well as an envelope for shaping the ADSR of each note.
//
// [instrument]
// |-->[envelope]
//     |-->[mixer]
//         |-->[oscillator]
//         |-->[oscillator]
//         |-->[oscillator]

class Instrument : public control::IControllable, public IGenerator {
public:
	Instrument() {
		m_tones = std::vector<ToneData>();
		m_mixer = std::make_shared<fx::Mixer>();
		m_envelope = std::make_unique<fx::Envelope>();

		m_envelope->set_source(m_mixer);
	}

	auto add_tone(std::shared_ptr<gen::IFrequencyGen> gen, float rel_pitch, float amplitude) {
		m_tones.push_back({ gen, rel_pitch });
		m_mixer->add_generator(gen, amplitude);
	}

	auto get_envelope() {
		return m_envelope.get();
	}

	auto set_frequency(float freq) {
		if (freq <= 0) {
			return;
		}

		for each (auto tone in m_tones)
		{
			tone.gen->set_freq(freq * tone.rel_pitch);
		}
	}

	auto set_note(std::string note) {
		auto freq = util::get_pitch_from_note(note);
		set_frequency(freq);
	}

	//
	// IControllable
	virtual auto start() -> void override {
		m_envelope->start();
	}

	virtual auto release() -> void override {
		m_envelope->release();
	}

	virtual auto end() -> void override {
		m_envelope->end();
	}

	//
	// IGenerator
	virtual auto do_tick(size_t id) -> TickResult override {
		// this should tick everything else if we're doing our job right
		return m_envelope->tick(id);
	}

	virtual auto calc_value() -> float override {
		return m_envelope->value();
	}

private:
	struct ToneData {
		std::shared_ptr<gen::IFrequencyGen> gen;
		float rel_pitch;
	};

	// Collection of wave generators with relative pitch and amplitude
	// describing the instrument
	std::vector<ToneData> m_tones;
	std::shared_ptr<fx::Mixer> m_mixer;
	std::unique_ptr<fx::Envelope> m_envelope;
};

}