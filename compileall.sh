#!/bin/bash

gcc -std=gnu99 -o keygen keygen.c
gcc -std=gnu99 -o otp_enc_d server.c
gcc -std=gnu99 -o otp_enc client.c

killall ABRT otp_enc_d

./otp_enc_d 57171 &