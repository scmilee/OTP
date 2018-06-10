#!/bin/bash
gcc -std=gnu99 -o keygen keygen.c
gcc -std=gnu99 -o otp_enc_d otp_enc_d.c
gcc -std=gnu99 -o otp_enc otp_enc.c
gcc -std=gnu99 -o otp_dec_d otp_dec_d.c
gcc -std=gnu99 -o otp_dec otp_dec.c

killall otp_enc_d
killall otp_dec_d