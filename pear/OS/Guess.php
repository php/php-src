<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2002 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Stig Bakken <ssb@fast.no>                                   |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

class OS_Guess
{
    function guess()
    {
        static $sysmap = array(
            'HP-UX' => 'hpux',
            'IRIX64' => 'irix',
            // Darwin?
        );
        static $cpumap = array(
            'i586' => 'i386',
            'i686' => 'i386',
        );
        $uname = php_uname();
        $parts = preg_split('/\s+/', trim($uname));
        $n = count($parts);

        $release = $fullversion = $machine = $processor = '';

        $sysname = $parts[0];
        $nodename = $parts[1];
        $cpu = $parts[$n-1];
        if ($cpu == 'unknown') {
            $cpu = $parts[$n-2];
        }

        // php_uname() function without args returns the same as 'uname -a'
        // some examples:
        //
        // RedHat Linux 7.1:
        // Linux host.example.com 2.4.2-2 #1 Sun Apr 8 20:41:30 EDT 2001 i686 unknown
        //
        // Debian Potato:
        // Linux host 2.4.17 #2 SMP Tue Feb 12 15:10:04 CET 2002 i686 unknown
        //
        // FreeBSD 3.3:
        // FreeBSD host.example.com 3.3-STABLE FreeBSD 3.3-STABLE #0: Mon Feb 21 00:42:31 CET 2000     root@example.com:/usr/src/sys/compile/CONFIG  i386
        //
        // FreeBSD 4.3:
        // FreeBSD host.example.com 4.3-RELEASE FreeBSD 4.3-RELEASE #1: Mon Jun 25 11:19:43 EDT 2001     root@example.com:/usr/src/sys/compile/CONFIG  i386
        //
        // FreeBSD 4.5:
        // FreeBSD host.example.com 4.5-STABLE FreeBSD 4.5-STABLE #0: Wed Feb  6 23:59:23 CET 2002     root@example.com:/usr/src/sys/compile/CONFIG  i386
        //
        // FreeBSD 4.5 w/uname from GNU shellutils:
        // FreeBSD host.example.com 4.5-STABLE FreeBSD 4.5-STABLE #0: Wed Feb  i386 unknown
        //
        // HP-UX 10:
        // HP-UX host B.10.10 A 9000/712 unknown
        //
        // AIX 4.3:
        // AIX host 3 4 000003531C00
        //
        // IRIX 6.5:
        // IRIX64 host 6.5 01091820 IP19 mips
        //
        // Solaris 8:
        // SunOS host.example.com 5.8 Generic_108528-12 sun4m sparc
        //
        switch ($sysname) {
            case 'AIX':
                $release = "$parts[3].$parts[2]";
                break;
            case 'Windows':
                $release = $parts[3];
                break;
            default:
                $release = preg_replace('/-.*/', '', $parts[2]);
                break;
        }


        if (isset($sysmap[$sysname])) {
            $sysname = $sysmap[$sysname];
        } else {
            $sysname = strtolower($sysname);
        }
        if (isset($cpumap[$cpu])) {
            $cpu = $cpumap[$cpu];
        }
        return "$sysname-$release-$cpu";
    }
}

/*
 * Local Variables:
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * End:
 */
?>