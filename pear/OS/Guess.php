<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 5                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2004 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 3.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available through the world-wide-web at the following url:           |
// | http://www.php.net/license/3_0.txt.                                  |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Stig Bakken <ssb@php.net>                                   |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

// {{{ uname examples

// php_uname() without args returns the same as 'uname -a', or a PHP-custom
// string for Windows.
// PHP versions prior to 4.3 return the uname of the host where PHP was built,
// as of 4.3 it returns the uname of the host running the PHP code.
//
// PC RedHat Linux 7.1:
// Linux host.example.com 2.4.2-2 #1 Sun Apr 8 20:41:30 EDT 2001 i686 unknown
//
// PC Debian Potato:
// Linux host 2.4.17 #2 SMP Tue Feb 12 15:10:04 CET 2002 i686 unknown
//
// PC FreeBSD 3.3:
// FreeBSD host.example.com 3.3-STABLE FreeBSD 3.3-STABLE #0: Mon Feb 21 00:42:31 CET 2000     root@example.com:/usr/src/sys/compile/CONFIG  i386
//
// PC FreeBSD 4.3:
// FreeBSD host.example.com 4.3-RELEASE FreeBSD 4.3-RELEASE #1: Mon Jun 25 11:19:43 EDT 2001     root@example.com:/usr/src/sys/compile/CONFIG  i386
//
// PC FreeBSD 4.5:
// FreeBSD host.example.com 4.5-STABLE FreeBSD 4.5-STABLE #0: Wed Feb  6 23:59:23 CET 2002     root@example.com:/usr/src/sys/compile/CONFIG  i386
//
// PC FreeBSD 4.5 w/uname from GNU shellutils:
// FreeBSD host.example.com 4.5-STABLE FreeBSD 4.5-STABLE #0: Wed Feb  i386 unknown
//
// HP 9000/712 HP-UX 10:
// HP-UX iq B.10.10 A 9000/712 2008429113 two-user license
//
// HP 9000/712 HP-UX 10 w/uname from GNU shellutils:
// HP-UX host B.10.10 A 9000/712 unknown
//
// IBM RS6000/550 AIX 4.3:
// AIX host 3 4 000003531C00
//
// AIX 4.3 w/uname from GNU shellutils:
// AIX host 3 4 000003531C00 unknown
//
// SGI Onyx IRIX 6.5 w/uname from GNU shellutils:
// IRIX64 host 6.5 01091820 IP19 mips
//
// SGI Onyx IRIX 6.5:
// IRIX64 host 6.5 01091820 IP19
//
// SparcStation 20 Solaris 8 w/uname from GNU shellutils:
// SunOS host.example.com 5.8 Generic_108528-12 sun4m sparc
//
// SparcStation 20 Solaris 8:
// SunOS host.example.com 5.8 Generic_108528-12 sun4m sparc SUNW,SPARCstation-20
//
// Mac OS X (Darwin)
// Darwin home-eden.local 7.5.0 Darwin Kernel Version 7.5.0: Thu Aug  5 19:26:16 PDT 2004; root:xnu/xnu-517.7.21.obj~3/RELEASE_PPC  Power Macintosh
//
// Mac OS X early versions
// 

// }}}

/* TODO:
 * - define endianness, to allow matchSignature("bigend") etc.
 */

class OS_Guess
{
    var $sysname;
    var $nodename;
    var $cpu;
    var $release;
    var $extra;

    function OS_Guess($uname = null)
    {
        list($this->sysname,
             $this->release,
             $this->cpu,
             $this->extra,
             $this->nodename) = $this->parseSignature($uname);
    }

    function parseSignature($uname = null)
    {
        static $sysmap = array(
            'HP-UX' => 'hpux',
            'IRIX64' => 'irix',
        );
        static $cpumap = array(
            'i586' => 'i386',
            'i686' => 'i386',
            'ppc' => 'powerpc',
        );
        if ($uname === null) {
            $uname = php_uname();
        }
        $parts = split('[[:space:]]+', trim($uname));
        $n = count($parts);

        $release = $machine = $cpu = '';
        $sysname = $parts[0];
        $nodename = $parts[1];
        $cpu = $parts[$n-1];
        $extra = '';
        if ($cpu == 'unknown') {
            $cpu = $parts[$n-2];
        }

        switch ($sysname) {
            case 'AIX':
                $release = "$parts[3].$parts[2]";
                break;
            case 'Windows':
                switch ($parts[1]) {
                    case '95/98':
                        $release = '9x';
                        break;
                    default:
                        $release = $parts[1];
                        break;
                }
                $cpu = 'i386';
                break;
            case 'Linux':
                $extra = $this->_detectGlibcVersion();
                // use only the first two digits from the kernel version
                $release = ereg_replace('^([[:digit:]]+\.[[:digit:]]+).*', '\1', $parts[2]);
                break;
            case 'Mac' :
                $sysname = 'darwin';
                $nodename = $parts[2];
                $release = $parts[3];
                if ($cpu == 'Macintosh') {
                    if ($parts[$n - 2] == 'Power') {
                        $cpu = 'powerpc';
                    }
                }
                break;
            case 'Darwin' :
                if ($cpu == 'Macintosh') {
                    if ($parts[$n - 2] == 'Power') {
                        $cpu = 'powerpc';
                    }
                }
                $release = ereg_replace('^([[:digit:]]+\.[[:digit:]]+).*', '\1', $parts[2]);
                break;
            default:
                $release = ereg_replace('-.*', '', $parts[2]);
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
        return array($sysname, $release, $cpu, $extra, $nodename);
    }

    function _detectGlibcVersion()
    {
        // Use glibc's <features.h> header file to
        // get major and minor version number:
        include_once "System.php";
        $tmpfile = System::mktemp("glibctest");
        $fp = fopen($tmpfile, "w");
        fwrite($fp, "#include <features.h>\n__GLIBC__ __GLIBC_MINOR__\n");
        fclose($fp);
        $cpp = popen("/usr/bin/cpp $tmpfile", "r");
        $major = $minor = 0;
        while ($line = fgets($cpp, 1024)) {
            if ($line{0} == '#' || trim($line) == '') {
                continue;
            }
            if (list($major, $minor) = explode(' ', trim($line))) {
                break;
            }
        }
        pclose($cpp);
        unlink($tmpfile);
        if (!($major && $minor) && is_link('/lib/libc.so.6')) {
            // Let's try reading the libc.so.6 symlink
            if (ereg('^libc-([.*])\.so$', basename(readlink('/lib/libc.so.6')), $matches)) {
                list($major, $minor) = explode('.', $matches);
            }
        }
        if (!($major && $minor)) {
            return '';
        }
        return "glibc{$major}.{$minor}";
    }

    function getSignature()
    {
        if (empty($this->extra)) {
            return "{$this->sysname}-{$this->release}-{$this->cpu}";
        }
        return "{$this->sysname}-{$this->release}-{$this->cpu}-{$this->extra}";
    }

    function getSysname()
    {
        return $this->sysname;
    }

    function getNodename()
    {
        return $this->nodename;
    }

    function getCpu()
    {
        return $this->cpu;
    }

    function getRelease()
    {
        return $this->release;
    }

    function getExtra()
    {
        return $this->extra;
    }

    function matchSignature($match)
    {
        if (is_array($match)) {
            $fragments = $match;
        } else {
            $fragments = explode('-', $match);
        }
        $n = count($fragments);
        $matches = 0;
        if ($n > 0) {
            $matches += $this->_matchFragment($fragments[0], $this->sysname);
        }
        if ($n > 1) {
            $matches += $this->_matchFragment($fragments[1], $this->release);
        }
        if ($n > 2) {
            $matches += $this->_matchFragment($fragments[2], $this->cpu);
        }
        if ($n > 3) {
            $matches += $this->_matchFragment($fragments[3], $this->extra);
        }
        return ($matches == $n);
    }

    function _matchFragment($fragment, $value)
    {
        if (strcspn($fragment, '*?') < strlen($fragment)) {
            $reg = '^' . str_replace(array('*', '?', '/'), array('.*', '.', '\\/'), $fragment) . '$';
            return eregi($reg, $value);
        }
        return ($fragment == '*' || !strcasecmp($fragment, $value));
    }

}
/*
 * Local Variables:
 * indent-tabs-mode: nil
 * c-basic-offset: 4
 * End:
 */
?>
