#!/usr/bin/sh

systemctl daemon-reload && systemctl enable $1 && systemctl start $1 && echo "Systemd calls done. Unit file: \"$1\""
