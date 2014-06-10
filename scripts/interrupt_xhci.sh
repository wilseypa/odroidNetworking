#!/bin/sh

sudo chmod 777 /sys/module/xhci_hcd/parameters/poll_option
sudo echo -n 0 > /sys/module/xhci_hcd/parameters/poll_option
