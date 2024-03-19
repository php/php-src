#!/bin/bash
lsb_release='/usr/bin/lsb_release'
architecture=`uname -i`
os='unknown'

if [ -x $lsb_release ]; then
    # we pull in default values, should work for both Debian and Ubuntu
    os=`$lsb_release -s -i | tr "[:upper:]" "[:lower:]"`

    if [ "$OS" == "centos" ]; then
        os_codename=`$lsb_release -s -r | sed -e 's/\..*//'`
    else
        os_codename=`$lsb_release -s -c | tr "[:upper:]" "[:lower:]"`
    fi

elif [ -r "/etc/redhat-release" ]; then
    # it's either RHEL or CentOS, which is fine
    os='centos'

    # instead of codename, we pull in release version ('6.3', '6.4', etc)
    os_codename=`sed s/.*release\ // /etc/redhat-release | sed s/\ .*//`
fi

echo $os_codename
