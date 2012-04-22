CC = gcc
CFLAGS = -Wall -ggdb -O0

kraken: kraken.c dns_enum.c host_manager.c
	$(CC) $(CFLAGS) -lcares -o kraken kraken.c host_manager.c dns_enum.c whois_lookup.c network_addr.c

clean:
	rm kraken
