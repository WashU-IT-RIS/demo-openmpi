#!/bin/sh

. $EGO_CONFDIR/profile.ego

if [ -f "$EGO_SERVERDIR/$1" ]; then
    case "$1" in 
        named) eval "$EGO_SERVERDIR/$@ -c $ESD_CONF/named/conf/named.conf" ;;
        *) eval "$EGO_SERVERDIR/$@" ;;
    esac
else
    exit 1
fi

