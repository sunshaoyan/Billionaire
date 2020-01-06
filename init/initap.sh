#!/bin/sh
### BEGIN INIT INFO
# Provides: horizon
# Required-Start:    $all
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:
# Short-Description: hobot ap
### END INIT INFO

sleep 1
cd /home/linaro/ap_base
sudo ./410c_bif_depoly.sh