#!/bin/bash

gcc -std=gnu99 -o keygen keygen.c
gcc -std=gnu99 -o otp_enc_d server.c
gcc -std=gnu99 -o otp_enc client.c
gcc -std=gnu99 -o otp_dec_d decserver.c
gcc -std=gnu99 -o otp_dec decclient.c

killall otp_enc_d
