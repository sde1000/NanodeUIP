#!/usr/bin/expect

spawn telnet 192.168.1.118
expect "NanodeUIP>"
send "mem\r"
expect "NanodeUIP>"
send "help\r"
expect "NanodeUIP>"
send "garbage\r"
expect "NanodeUIP>"
send "exit\r"
expect "NanodeUIP>"


