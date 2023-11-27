#include "sync.h"
#include "ieee80211.h"

void apple_p2p_sync_state_init(struct apple_p2p_sync_state *state, uint64_t now) {
	state->last_update = now;
	state->aw_counter = 0;
	state->aw_period = 16;
	state->presence_mode = 4;

	state->meas_err = 0;
	state->meas_total = 0;
}

uint16_t apple_p2p_sync_next_aw_tu(uint64_t now_usec, const struct apple_p2p_sync_state *state) {
	uint64_t eaw_period = state->presence_mode * state->aw_period;
	uint64_t time_since = ieee80211_usec_to_tu(now_usec - state->last_update);
	uint64_t next_aw_tu = eaw_period - (time_since % eaw_period);
	return (uint16_t) next_aw_tu;
}

uint64_t apple_p2p_sync_next_aw_us(uint64_t now_usec, const struct apple_p2p_sync_state *state) {
	uint64_t eaw_period = ieee80211_tu_to_usec(state->presence_mode * state->aw_period);
	uint64_t time_since = now_usec - state->last_update;
	uint64_t next_aw_us = eaw_period - (time_since % eaw_period);
	return next_aw_us;
}

uint16_t apple_p2p_sync_current_aw(uint64_t now_usec, const struct apple_p2p_sync_state *state) {
	uint64_t eaw_period = state->presence_mode * state->aw_period;
	uint64_t time_since = ieee80211_usec_to_tu(now_usec - state->last_update);
	uint64_t current_aw = state->aw_counter + /* last counter */
	                      (time_since % eaw_period) / state->aw_period + /* within EAW */
	                      state->presence_mode * (time_since / eaw_period); /* correction for EAWs */
	return (uint16_t) current_aw;
};

uint16_t apple_p2p_sync_current_eaw(uint64_t now_usec, const struct apple_p2p_sync_state *state) {
	return (apple_p2p_sync_current_aw(now_usec, state) / state->presence_mode);
}

int64_t apple_p2p_sync_error_tu(uint64_t now_usec, uint16_t time_to_next_aw, uint16_t aw_counter,
                           const struct apple_p2p_sync_state *state) {
	return ((aw_counter / state->presence_mode - apple_p2p_sync_current_eaw(now_usec, state)) *
	        state->presence_mode * state->aw_period) -
	       (time_to_next_aw - apple_p2p_sync_next_aw_tu(now_usec, state));
}

void apple_p2p_sync_update_last(uint64_t now_usec, uint16_t time_to_next_aw, uint16_t aw_counter,
                           struct apple_p2p_sync_state *state) {
	uint64_t eaw_period = state->presence_mode * state->aw_period;
	state->last_update = now_usec - ieee80211_tu_to_usec(eaw_period - time_to_next_aw);
	state->aw_counter = aw_counter & 0xfffc; /* mask last two bits, effectively 'aw_counter/4*4' */
}
