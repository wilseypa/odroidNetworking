#!/bin/sh

sudo chmod 777 /sys/module/ehci_hcd/parameters/poll_option
sudo echo -n 1 > /sys/module/ehci_hcd/parameters/poll_option
