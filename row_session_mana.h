#ifndef ROW_SESSION_MANA_H
#define ROW_SESSION_MANA_H

#include "raw_sock_util.h"

raw_sock_session_info* addSession(struct ethhdr * ethh, struct ip *iph, struct tcphdr *tcph, raw_sock_local_env_conf* pLocalEnv);
raw_sock_session_info* getSession(struct ip *iph, struct tcphdr *tcph, raw_sock_local_env_conf* pLocalEnv);

#endif
