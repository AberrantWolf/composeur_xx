#include "Sequence.h"

using namespace compx::music;

float Note::m_global_early_release = THIRTYSECOND_NOTE.duration;

auto Sequence::do_tick(size_t id) -> compx::TickResult {
	if (m_sequence.size() == 0) {
		return TickResult::COMPLETE;
	}

	m_time += m_time_scale * util::SAMPLE_RATE.seconds_per_step();

	auto step = m_sequence.front();
	if (m_time >= step.delay) {
		m_time -= step.delay;

		m_instrument->set_note(step.pitch);

		switch (step.evt) {
		case NoteEvent::PLAY:
			m_instrument->start();
			break;
		case NoteEvent::RELEASE:
			m_instrument->release();
			break;
		case NoteEvent::REST:
			// shouldn't happen yet?
			break;
		case NoteEvent::SLUR:
			// nothing yet
			break;
		}

		m_sequence.pop();
	}

	m_instrument->tick(id);

	return TickResult::OK;
}