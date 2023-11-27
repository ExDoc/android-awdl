#ifndef __IO_H__
#define __IO_H__

#include <stdint.h>
#undef BPF_STMT
#undef BPF_JUMP
#include <pcap/pcap.h>
#include <net/if.h>
#include <limits.h>

#ifdef __APPLE__
#include <net/ethernet.h>
#else
#include <net/ethernet.h>
#include <netinet/ether.h>
#include "../libpcap/pcap/pcap.h"

#endif

struct io_state {
	pcap_t *wlan_handle;
	char wlan_ifname[PATH_MAX]; /* name of WLAN iface */
	int wlan_ifindex;     /* index of WLAN iface */
	char host_ifname[IFNAMSIZ]; /* name of host iface */
	int host_ifindex;     /* index of host iface */
	struct ether_addr if_ether_addr; /* MAC address of WLAN and host iface */
	int wlan_fd;
	int host_fd;
	char *dumpfile;
	char wlan_no_monitor_mode;
	int wlan_is_file;
};

int io_state_init(struct io_state *state, const char *wlan, const char *host, const struct ether_addr *bssid_filter);

void io_state_free(struct io_state *state);

int wlan_send(const struct io_state *state, const uint8_t *buf, int len);

int host_send(const struct io_state *state, const uint8_t *buf, int len);

int host_recv(const struct io_state *state, uint8_t *buf, int *len);

#endif /* __IO_H__ */
