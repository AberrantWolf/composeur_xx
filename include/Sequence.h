#pragma once

#include <queue>

#include "Instrument.h"
#include "Interfaces/IGenerator.h"
#include "Util.h"

#define NOTE_DURATION(name, division)\
	NoteTiming name##_NOTE { division, 1 / float(division) }

namespace compx {
namespace music {

struct NoteTiming {
	uint32_t interval;
	float duration;
};

constexpr NOTE_DURATION(WHOLE, 1);
constexpr NOTE_DURATION(HALF, 2);
constexpr NOTE_DURATION(QUARTER, 4);
constexpr NOTE_DURATION(EIGHTH, 8);
constexpr NOTE_DURATION(SIXTEENTH, 16);
constexpr NOTE_DURATION(THIRTYSECOND, 32);

class Note {
	friend class Sequence;

public:
	// Notes release some amount before their time ends. This is easier to set globally
	static void set_early_release(float ratio) {
		m_global_early_release = ratio;
	}

	Note(std::string pitch, const NoteTiming& info)
		: m_base_duration(info.duration)
		, m_dot_count(0)
		, m_early_release(m_global_early_release)
		, m_pitch(pitch) {
		correct_early_release();
	}

	Note(std::string pitch, const NoteTiming& info, uint32_t dot_count)
		: m_base_duration(info.duration)
		, m_dot_count(dot_count)
		, m_early_release(m_global_early_release)
		, m_pitch(pitch) {
		correct_early_release();
	}

	//Note(std::string pitch, const NoteTiming& info, float early_release)
	//	: m_base_duration(info.duration)
	//	, m_dot_count(0)
	//	, m_early_release(early_release)
	//	, m_pitch(pitch) {
	//	correct_early_release();
	//}

	Note(std::string pitch, const NoteTiming& info, uint32_t dot_count, float early_release)
		: m_base_duration(info.duration)
		, m_dot_count(dot_count)
		, m_early_release(early_release)
		, m_pitch(pitch) {
		correct_early_release();
	}

private:
	std::string m_pitch;
	float m_base_duration;
	float m_dot_count;
	float m_early_release;
	static float m_global_early_release;

	auto correct_early_release() -> void {
		if (m_base_duration <= 0) {
			// This should be an error state
			return;
		}

		while (m_early_release >= m_base_duration) {
			m_early_release *= 0.5f;
		}
	}

	// Return how long the note actually plays
	auto duration() -> float {
		if (m_dot_count == 0) {
			return m_base_duration - m_early_release;
		}

		float duration = m_base_duration;

		auto dot_time = duration * 0.5f;
		for (int i = 0; i < m_dot_count; ++i) {
			duration += dot_time;
			dot_time *= 0.5f;
		}

		return duration - m_early_release;
	}

	// How much delay to insert after releasing before the next note could start
	auto remaining() -> float {
		return m_early_release;
	}
};

struct TimeSignature {
	uint8_t count;
	uint8_t beat;
};

class Sequence : public IGenerator {
public:
	Sequence()
		: m_sequence()
		, m_signature({ 4, 4 })
		, m_tempo(120.0f)
		, m_time(0.0f) {
		initialize();
	}
	Sequence(float tempo)
		: m_sequence()
		, m_signature({4, 4})
		, m_tempo(tempo)
		, m_time(0.0f) {
		initialize();
	}
	Sequence(TimeSignature signature)
		: m_sequence()
		, m_signature(signature)
		, m_tempo(120.0f)
		, m_time(0.0f) {
		initialize();
	}
	Sequence(float tempo, TimeSignature signature)
		: m_sequence()
		, m_signature(signature)
		, m_tempo(tempo)
		, m_time(0.0f) {
		initialize();
	}

	auto set_tempo(float bpm) {
		m_tempo = bpm;
		// recalc time per sample
	}

	auto set_signature(TimeSignature sig) {
		m_signature = sig;
		// recalc time per sample
	}

	auto set_instrument(std::shared_ptr<Instrument> instrument) {
		m_instrument = instrument;
	}

	auto play_note(Note& note) {
		m_sequence.back().pitch = note.m_pitch;
		m_sequence.back().evt = NoteEvent::PLAY;
		m_sequence.emplace(NoteEvent::RELEASE, note.m_pitch, note.duration());
		m_sequence.emplace(NoteEvent::REST, "", note.remaining());
	}

	auto rest_for(NoteTiming& timing) -> float {
		m_sequence.back().delay += timing.duration;
	}

	auto rest_until_next() {
		// TODO...
	}

	//
	// IGenerator
	virtual auto do_tick(size_t id)->TickResult override;

	virtual auto calc_value() -> float override {
		return m_instrument->value();
	}

private:
	enum class NoteEvent {
		PLAY,
		RELEASE,
		REST,
		SLUR,
	};

	struct TimedNoteEvent {
		NoteEvent evt;
		std::string pitch;
		float delay;

		TimedNoteEvent(NoteEvent evt, std::string pitch, float delay)
			: evt(evt)
			, pitch(pitch)
			, delay(delay) {}
	};

	float m_tempo; // bpm
	TimeSignature m_signature; // determine which note gets "the beat"
	float m_time_scale;
	float m_time;
	std::shared_ptr<Instrument> m_instrument;
	std::queue<TimedNoteEvent> m_sequence;

	auto solve_time_per_sample() {
		// 60 bpm = 1
		// 4/4, 0.25 = 1

		// 1 sec = bpm / 1 min
		// 1 sec = note_ratio * beat_note

		// @ 120, 1/4 note, 1/4 should be 2.0 * 4
		// @ 60, 1/4 note, 1/4 should be 1.0 * 4
		// @ 60, 1/8 note, 1/4 should be 0.5 * 4
		m_time_scale = m_tempo / 60.0f / m_signature.beat;
	}

	auto initialize() -> void {
		m_sequence.emplace(NoteEvent::REST, "", 0.0f);
		solve_time_per_sample();
	}
};

} // namespace music
} // namespace compx