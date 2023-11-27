#include "channel.h"
#include "ieee80211.h"

void apple_p2p_chanseq_init(struct apple_p2p_chan *seq) {
	for (int i = 0; i < APPLE_P2P_CHANSEQ_LENGTH; i++, seq++) {
		if (i < 8)
			*seq = CHAN_OPCLASS_149;
		else
			*seq = CHAN_OPCLASS_6;
	}
}

void apple_p2p_chanseq_init_idle(struct apple_p2p_chan *seq) {
	for (int i = 0; i < APPLE_P2P_CHANSEQ_LENGTH; i++, seq++) {
		switch (i) {
			case 8:
				*seq = CHAN_OPCLASS_6;
				break;
			case 0:
			case 9:
			case 10:
				*seq = CHAN_OPCLASS_149;
				break;
			default:
				*seq = CHAN_NULL;
				break;
		}
	}
}

void apple_p2p_chanseq_init_static(struct apple_p2p_chan *seq, const struct apple_p2p_chan *chan) {
	for (int i = 0; i < APPLE_P2P_CHANSEQ_LENGTH; i++, seq++) {
		*seq = *chan;
	}
}

uint8_t apple_p2p_chan_num(struct apple_p2p_chan chan, enum apple_p2p_chan_encoding enc) {
	switch (enc) {
		case APPLE_P2P_CHAN_ENC_SIMPLE:
			return chan.simple.chan_num;
		case APPLE_P2P_CHAN_ENC_LEGACY:
			return chan.legacy.chan_num;
		case APPLE_P2P_CHAN_ENC_OPCLASS:
			return chan.opclass.chan_num;
		default:
			return 0; /* unknown encoding */
	}
}

int apple_p2p_chan_encoding_size(enum apple_p2p_chan_encoding enc) {
	switch (enc) {
		case APPLE_P2P_CHAN_ENC_SIMPLE:
			return 1;
		case APPLE_P2P_CHAN_ENC_LEGACY:
		case APPLE_P2P_CHAN_ENC_OPCLASS:
			return 2;
		default:
			return -1; /* unknown encoding */
	}
}

/* adapted from iw/util.c */
int ieee80211_channel_to_frequency(int chan) {
	/* see 802.11 17.3.8.3.2 and Annex J
	 * there are overlapping channel numbers in 5GHz and 2GHz bands */
	if (chan <= 0)
		return 0; /* not supported */

	/* 2 GHz band */
	if (chan == 14)
		return 2484;
	else if (chan < 14)
		return 2407 + chan * 5;

	/* 5 GHz band */
	if (chan < 32)
		return 0; /* not supported */

	if (chan >= 182 && chan <= 196)
		return 4000 + chan * 5;
	else
		return 5000 + chan * 5;

}

/* from iw/util.c */
int ieee80211_frequency_to_channel(int freq) {
	/* see 802.11-2007 17.3.8.3.2 and Annex J */
	if (freq == 2484)
		return 14;
	else if (freq < 2484)
		return (freq - 2407) / 5;
	else if (freq >= 4910 && freq <= 4980)
		return (freq - 4000) / 5;
	else if (freq <= 45000) /* DMG band lower limit */
		return (freq - 5000) / 5;
	else if (freq >= 58320 && freq <= 64800)
		return (freq - 56160) / 2160;
	else
		return 0;
}
