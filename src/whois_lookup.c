// whois_lookup.c
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above
//   copyright notice, this list of conditions and the following disclaimer
//   in the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of SecureState Consulting nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "kraken.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "whois_lookup.h"
#include "host_manager.h"
#include "network_addr.h"

int whois_parse_response_arin(char *raw_resp, whois_response *who_resp) {
	int szResp = 0;
	int szData = 0;
	char *start_tag = NULL;
	char *pCur = NULL;

	szResp = strlen(raw_resp);
	pCur = raw_resp;

	/* first we need to check for # start and # end directives so we can skip to the last one */
	while (pCur < (raw_resp + szResp)) {
		if (strncasecmp(pCur, "# start", 7) == 0) {
			 start_tag = pCur;
		}
		pCur = strchr(pCur, '\n');
		if (pCur == NULL) {
			break;
		}
		pCur += 1;
	}

	if (start_tag == NULL) {
		pCur = raw_resp;
	} else {
		pCur = start_tag;
	}

	while (pCur < (raw_resp + szResp)) {
		szData = 0;
		pCur += 1;		/* advance the cursor once to skip the newline */
		if ((*pCur == '#') || (*pCur == '\n')) {
			continue;	/* skip comments */
		}
		if (strncasecmp(pCur, "cidr: ", 6) == 0) {
			pCur += 6;
			while ((*pCur == ' ') && (pCur < (raw_resp + szResp))) {
				pCur += 1;
			}
			while ((((*(pCur + szData) > 47) && (*(pCur + szData) < 58)) || (*(pCur + szData) == '.') || (*(pCur + szData) == '/')) && (pCur < (raw_resp + szResp)) && (szData <= WHOIS_SZ_DATA_S)) {
				szData += 1;
			}
			if (*who_resp->cidr_s == '\0') {
				if (szData < WHOIS_SZ_DATA_S) {
					strncpy(who_resp->cidr_s, pCur, szData);
				} else {
					strncpy(who_resp->cidr_s, pCur, WHOIS_SZ_DATA_S);
				}
			}
			pCur += szData;
		} else if (strncasecmp(pCur, "netname: ", 9) == 0) {
			pCur += 9;
			while ((*pCur == ' ') && (pCur < (raw_resp + szResp)) && (szData <= WHOIS_SZ_DATA)) {
				pCur += 1;
			}
			while ((*(pCur + szData) != '\n') && (pCur < (raw_resp + szResp))) {
				szData += 1;
			}
			if (*who_resp->netname == '\0') {
				if (szData < WHOIS_SZ_DATA) {
					strncpy(who_resp->netname, pCur, szData);
				} else {
					strncpy(who_resp->netname, pCur, WHOIS_SZ_DATA);
				}
			}
			pCur += szData;
		} else if (strncasecmp(pCur, "orgname: ", 9) == 0) {
			pCur += 9;
			while ((*pCur == ' ') && (pCur < (raw_resp + szResp)) && (szData <= WHOIS_SZ_DATA)) {
				pCur += 1;
			}
			while ((*(pCur + szData) != '\n') && (pCur < (raw_resp + szResp))) {
				szData += 1;
			}
			if (*who_resp->orgname == '\0') {
				if (szData < WHOIS_SZ_DATA) {
					strncpy(who_resp->orgname, pCur, szData);
				} else {
					strncpy(who_resp->orgname, pCur, WHOIS_SZ_DATA);
				}
			}
			pCur += szData;
		} else if (strncasecmp(pCur, "custname: ", 10) == 0) {
			pCur += 10;
			while ((*pCur == ' ') && (pCur < (raw_resp + szResp)) && (szData <= WHOIS_SZ_DATA)) {
				pCur += 1;
			}
			while ((*(pCur + szData) != '\n') && (pCur < (raw_resp + szResp))) {
				szData += 1;
			}
			if (*who_resp->orgname == '\0') {
				if (szData < WHOIS_SZ_DATA) {
					strncpy(who_resp->orgname, pCur, szData);
				} else {
					strncpy(who_resp->orgname, pCur, WHOIS_SZ_DATA);
				}
			}
			pCur += szData;
		} else if (strncasecmp(pCur, "regdate: ", 9) == 0) {
			pCur += 9;
			while ((*pCur == ' ') && (pCur < (raw_resp + szResp)) && (szData <= WHOIS_SZ_DATA_S)) {
				pCur += 1;
			}
			while ((*(pCur + szData) != '\n') && (pCur < (raw_resp + szResp))) {
				szData += 1;
			}
			if (*who_resp->regdate_s == '\0') {
				if (szData < WHOIS_SZ_DATA_S) {
					strncpy(who_resp->regdate_s, pCur, szData);
				} else {
					strncpy(who_resp->regdate_s, pCur, WHOIS_SZ_DATA_S);
				}
			}
			pCur += szData;
		} else if (strncasecmp(pCur, "updated: ", 9) == 0) {
			pCur += 9;
			while ((*pCur == ' ') && (pCur < (raw_resp + szResp)) && (szData <= WHOIS_SZ_DATA_S)) {
				pCur += 1;
			}
			while ((*(pCur + szData) != '\n') && (pCur < (raw_resp + szResp))) {
				szData += 1;
			}
			if (*who_resp->updated_s != '\0') {
				if (szData < WHOIS_SZ_DATA_S) {
					strncpy(who_resp->updated_s, pCur, szData);
				} else {
					strncpy(who_resp->updated_s, pCur, WHOIS_SZ_DATA_S);
				}
			}
			pCur += szData;
		} else {
			pCur = strchr(pCur, '\n');
			if (pCur == NULL) {
				break;
			}
		}
	}
	return 0;
}

int whois_parse_response_ripe(char *raw_resp, whois_response *who_resp) {
	int szResp = 0;
	int szData = 0;
	char *pCur = NULL;
	char *iplow = NULL;
	char *iphigh = NULL;
	struct network_addr cidrn;

	szResp = strlen(raw_resp);
	pCur = raw_resp;
	while (pCur < (raw_resp + szResp)) {
		szData = 0;
		pCur += 1;		/* advance the cursor once to skip the newline */
		if ((*pCur == '#') || (*pCur == '\n')) {
			continue;	/* skip comments */
		}
		if (strncasecmp(pCur, "inetnum: ", 9) == 0) {
			pCur += 9;
			while ((*pCur == ' ') && (pCur < (raw_resp + szResp))) {
				pCur += 1;
			}
			iplow = pCur;
			while ((((*pCur > 47) && (*pCur < 58)) || (*pCur == '.')) && (pCur < (raw_resp + szResp))) {
				pCur += 1;
			}
			*pCur = '\0';
			pCur += 1;
			while ((*pCur == ' ') || (*pCur == '-')) {
				pCur += 1;
			}
			if ((*pCur != '\n') && (*pCur != '\0')) {
				iphigh = pCur;
				while ((*pCur != '\n') && (*pCur != '\0') && (pCur < (raw_resp + szResp))) {
					pCur += 1;
				}
				*pCur = '\0';
				pCur += 1;
			}
			if ((iplow != NULL) && (iphigh != NULL)) {
				if (netaddr_range_str_to_nwk(&cidrn, iplow, iphigh)) {
					netaddr_nwk_to_cidr_str(&cidrn, who_resp->cidr_s, WHOIS_SZ_DATA_S);
				}
			}
			iplow[strlen(iplow)] = ' ';
			iplow = NULL;
			iphigh[strlen(iphigh)] = '\n';
			iphigh = NULL;
		} else if (strncasecmp(pCur, "netname: ", 9) == 0) {
			pCur += 9;
			while ((*pCur == ' ') && (pCur < (raw_resp + szResp)) && (szData <= WHOIS_SZ_DATA)) {
				pCur += 1;
			}
			while ((*(pCur + szData) != '\n') && (pCur < (raw_resp + szResp))) {
				szData += 1;
			}
			if (szData < WHOIS_SZ_DATA) {
				strncpy(who_resp->netname, pCur, szData);
			} else {
				strncpy(who_resp->netname, pCur, WHOIS_SZ_DATA);
			}
			pCur += szData;
		} else if (strncasecmp(pCur, "descr: ", 7) == 0) {
			pCur += 7;
			while ((*pCur == ' ') && (pCur < (raw_resp + szResp)) && (szData <= WHOIS_SZ_DATA)) {
				pCur += 1;
			}
			while ((*(pCur + szData) != '\n') && (pCur < (raw_resp + szResp))) {
				szData += 1;
			}
			if (strlen(who_resp->description) == 0) {
				if (szData < WHOIS_SZ_DATA) {
					strncpy(who_resp->description, pCur, szData);
				} else {
					strncpy(who_resp->description, pCur, WHOIS_SZ_DATA);
				}
			}
			pCur += szData;
		} else {
			pCur = strchr(pCur, '\n');
			if (pCur == NULL) {
				break;
			}
		}
	}
	return 0;
}

int whois_lookup_ip(struct in_addr *ip, whois_response *who_resp) {
	char raw_resp[WHOIS_SZ_RESP];
	char ipstr[INET6_ADDRSTRLEN];
	int retVal = 0;

	inet_ntop(AF_INET, ip, ipstr, sizeof(ipstr));

	memset(who_resp, '\0', sizeof(whois_response));
	retVal = whois_raw_lookup(WHOIS_REQ_TYPE_IP, WHOIS_SRV_ARIN, ipstr, raw_resp);
	if (retVal != 0) {
		retVal = whois_raw_lookup(WHOIS_REQ_TYPE_IP, WHOIS_SRV_RIPE, ipstr, raw_resp);
		if (retVal == 0) {
			whois_parse_response_ripe(raw_resp, who_resp);
		} else {
			return retVal;
		}
	} else {
		whois_parse_response_arin(raw_resp, who_resp);
	}

	if (memcmp(who_resp->orgname, "RIPE", 4) == 0) {
		retVal = whois_raw_lookup(WHOIS_REQ_TYPE_IP, WHOIS_SRV_RIPE, ipstr, raw_resp);
		if (retVal == 0) {
			memset(who_resp, '\0', sizeof(whois_response));
			whois_parse_response_ripe(raw_resp, who_resp);
		} else {
			return retVal;
		}
	}
	return 0;
}

int whois_raw_lookup(int req_type, int target_server, char *request, char *response) {
	/*
	 * Returns 0 on Success
	 * Returns 1 on Error
	 */
	int sock;
	int szReq = 0;
	int szResp = 0;
	int tmpSzResp = 0;
	int addrCtr = 0;
	char reqBuffer[WHOIS_SZ_REQ];
	struct sockaddr_in dest_addr;
	fd_set fdRead;
	struct timeval timeout;
	struct hostent *server_info = NULL;

	memset(reqBuffer,  '\0', sizeof(WHOIS_SZ_REQ));
	memset(response,   '\0', sizeof(WHOIS_SZ_RESP));
	memset(&dest_addr, '\0', sizeof(dest_addr));

	if (target_server == WHOIS_SRV_ARIN) {
		server_info = gethostbyname(WHOIS_SRV_HOST_ARIN);
		if (req_type == WHOIS_REQ_TYPE_IP) {
			snprintf(reqBuffer, sizeof(reqBuffer), "n + %s\r\n", request);
		} else if (req_type == WHOIS_REQ_TYPE_HOST) {
			snprintf(reqBuffer, sizeof(reqBuffer), "%s\r\n", request);
		} else {
			LOGGING_QUICK_ERROR("kraken.whois", "could not determine WHOIS request type")
			return 2;
		}
	} else if (target_server == WHOIS_SRV_RIPE) {
		server_info = gethostbyname(WHOIS_SRV_HOST_RIPE);
		if (req_type == WHOIS_REQ_TYPE_IP) {
			snprintf(reqBuffer, sizeof(reqBuffer), "%s\r\n", request);
		} else if (req_type == WHOIS_REQ_TYPE_HOST) {
			snprintf(reqBuffer, sizeof(reqBuffer), "%s\r\n", request);
		} else {
			LOGGING_QUICK_ERROR("kraken.whois", "could not determine WHOIS request type")
			return 2;
		}
	}
	if (server_info == NULL) {
		LOGGING_QUICK_ERROR("kraken.whois", "could not lookup the IP of the whois server")
		return 3;
	}

	sock = socket(server_info->h_addrtype, SOCK_STREAM, 0);
	if (sock == -1) {
		LOGGING_QUICK_ERROR("kraken.whois", "could not allocate a socket")
		return 1;
	}

	dest_addr.sin_family = server_info->h_addrtype;
	dest_addr.sin_port = htons(WHOIS_PORT);

	szReq = strlen(reqBuffer);

	while (server_info->h_addr_list[addrCtr] != NULL) {
		memcpy(&dest_addr.sin_addr, server_info->h_addr_list[addrCtr], sizeof(struct in_addr));
		if (connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == 0) {
			break;
		}
		addrCtr += 1;
	}
	if (addrCtr) {
		LOGGING_QUICK_ERROR("kraken.whois", "could not connect to WHOIS server")
		return 4;
	}
	if (send(sock, reqBuffer, szReq, 0) != szReq) {
		LOGGING_QUICK_ERROR("kraken.whois", "failed to send the expected amount of data")
		return 5;
	}

	FD_ZERO(&fdRead);
	FD_SET(sock, &fdRead);
	timeout.tv_sec = WHOIS_TIMEOUT_SEC;
	timeout.tv_usec = WHOIS_TIMEOUT_USEC;

	while (szResp < WHOIS_SZ_RESP) {
		select(sock + 1, &fdRead, NULL, NULL, &timeout);
		if (FD_ISSET(sock, &fdRead) == 0) {
			if (szResp > 0) {
				return 0;
			} else {
				return 5;
			}
		}
		tmpSzResp = recv(sock, &response[szResp], (WHOIS_SZ_RESP - szResp), 0);
		szResp += tmpSzResp;
		if ((tmpSzResp == 0) && (szResp > 0)) {
			break;
		} else if ((tmpSzResp == 0) && (szResp == 0)) {
			return 5;
		}
	}
	return 0;
}

int whois_fill_host_manager(host_manager *c_host_manager) {
	/* Sync data (get unknown records)*/
	host_iter host_i;
	single_host_info *c_host;
	whois_record tmp_who_resp;
	whois_record *cur_who_resp = NULL;
	int ret_val = 0;
	char ipstr[INET6_ADDRSTRLEN];

	host_manager_iter_host_init(c_host_manager, &host_i);
	while (host_manager_iter_host_next(c_host_manager, &host_i, &c_host)) {
		if (c_host->whois_data != NULL) {
			continue;
		}
		host_manager_get_whois_by_addr(c_host_manager, &c_host->ipv4_addr, &cur_who_resp);
		if (cur_who_resp != NULL) {
			c_host->whois_data = cur_who_resp;
			continue;
		}
		ret_val = whois_lookup_ip(&c_host->ipv4_addr, &tmp_who_resp);
		if (ret_val == 0) {
			inet_ntop(AF_INET, &c_host->ipv4_addr, ipstr, sizeof(ipstr));
			logging_log("kraken.whois", LOGGING_INFO, "got whois record for %s, %s", ipstr, tmp_who_resp.cidr_s);
			host_manager_add_whois(c_host_manager, &tmp_who_resp);
			host_manager_get_whois_by_addr(c_host_manager, &c_host->ipv4_addr, &cur_who_resp);
			c_host->whois_data = cur_who_resp;
		}
	}
	return 0;
}

char *whois_get_best_name(whois_record *who_data) {
	if (strlen(who_data->orgname) > strlen(who_data->description)) {
		return who_data->orgname;
	} else if (strlen(who_data->description) > 0) {
		return who_data->description;
	}
	return who_data->netname;
}
