
    int socket_descriptor;
        struct sockaddr_in sin;
	    socklen_t sin_len = sizeof(struct sockaddr_in);

	        memset(&sin, 0, sizeof(sin));
		    sin.sin_family=AF_INET;
		        sin.sin_addr.s_addr = inet_addr("192.168.0.4");
			    sin.sin_port=htons(17018);

			        socket_descriptor=socket(AF_INET, SOCK_STREAM, 0);
				    tc8_log("Tc8ArpUtil::%s() socket_descriptor %d", __FUNCTION__, socket_descriptor);
				        if(socket_descriptor < 0)
	    {
		            tc8_log("Tc8ArpUtil::%s() socket %d(%s)", __FUNCTION__, errno, strerror(errno));
			            return;
				        }
    auto_close_fd _pFdServer(&socket_descriptor, Del_close_file);
        int ret = bind(socket_descriptor, (const struct sockaddr *) &sin, sin_len);
	    if(ret < 0)
	    {
		            tc8_log("Tc8ArpUtil::%s() bind %d(%s)", __FUNCTION__, errno, strerror(errno));
			            return;
				        }

    ret = listen(socket_descriptor, 1);
        if(ret < 0)
	    {
		            tc8_log("Tc8ArpUtil::%s() listen %d(%s)", __FUNCTION__, errno, strerror(errno));
			            return;
				        }
    tc8_log("Tc8ArpUtil::%s() wait accept...", __FUNCTION__);
        int client_hd = accept(socket_descriptor, NULL, NULL);
	    if(client_hd < 0)
	    {
		            tc8_log("Tc8ArpUtil::%s() listen %d(%s)", __FUNCTION__, errno, strerror(errno));
			            return;
				        }
    tc8_log("Tc8ArpUtil::%s() client_hd %d", __FUNCTION__, client_hd);
        {
		        auto_close_fd _pFdClient(&client_hd, Del_close_file);
			        char tmp[5];
				        do
						        {
								            ret = read(client_hd, tmp, 5);
									                if(ret <= 0)
												            {
														                    tc8_log("Tc8ArpUtil::%s() read %d(%s)", __FUNCTION__, errno, strerror(errno));
																                }
											        } while(ret > 0);
					    }

