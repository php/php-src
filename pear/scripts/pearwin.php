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
// |          Tomas V.V.Cox <cox@idecnet.com>                             |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR.php';
require_once 'Console/Getopt.php';

error_reporting(E_ALL ^ E_NOTICE);

$options = Console_Getopt::getopt($argv, "h?v:e:p:d:");
if (PEAR::isError($options)) {
    usage($options);
}

$opts = $options[0];
foreach ($opts as $opt) {
    $param = $opt[1];
    switch ($opt[0]) {
        case 'v':
            $verbose = $param;
            break;
        case 'e':
            if ($param{0} != getenv('DIRECTORY_SEPARATOR')) {
                usage (new PEAR_Error("no absolute path (eg. /usr/lib/php)\n"));
            }
            $ext_dir = $param;
            break;
        case 'p':
            if ($param{0} != getenv('DIRECTORY_SEPARATOR')) {
                usage (new PEAR_Error("no absolute path (eg. /usr/lib/php)\n"));
            }
            $script_dir = $param;
            break;
        case 'd':
            if ($param{0} != getenv('DIRECTORY_SEPARATOR')) {
                usage (new PEAR_Error("no absolute path (eg. /usr/lib/php)\n"));
            }
            $doc_dir = $param;
            break;
    }
}

$verbose    = (isset($verbose)) ? $verbose : 1;
$script_dir = (isset($script_dir)) ? $script_dir : getenv('PEAR_INSTALL_DIR');
$ext_dir    = (isset($ext_dir)) ? $ext_dir : getenv('PEAR_EXTENSION_DIR');
$doc_dir    = (isset($doc_dir)) ? $doc_dir : '';

PEAR::setErrorHandling(PEAR_ERROR_PRINT);
$command = $options[1][1];

switch ($command) {
    case 'install':
        include_once 'PEAR/Installer.php';
        $package = $options[1][2];
        $installer =& new PEAR_Installer($script_dir, $ext_dir, $doc_dir);
        $installer->debug = $verbose;
        if (PEAR::isError($installer->Install($package))) {
            print "\ninstall failed\n";
        } else {
            print "install ok\n";
        }
        break;
    case 'uninstall':
        include_once 'PEAR/Installer.php';
        $package = $options[1][2];
        $installer =& new PEAR_Installer($script_dir, $ext_dir, $doc_dir);
        $installer->debug = $verbose;
        if (PEAR::isError($installer->uninstall($package))) {
            print "\nuninstall failed\n";
        } else {
            print "uninstall ok\n";
        }
        break;
    case 'list-installed':
        include_once 'PEAR/Registry.php';
        $reg = new PEAR_Registry($script_dir);
        $installed = $reg->packageInfo();
        $i = $j = 0;
        print("Installed packages:\n");
        foreach ($installed as $package) {
            if ($i++ % 20 == 0) {
                if ($j++ > 0) {
                    print "\n";
                }
                printf("%-20s %-10s %s\n",
                       "Package", "Version", "State");
                print str_repeat("-", 75)."\n";
            }
            printf("%-20s %-10s %s\n", $package['package'],
                   $package['version'], $package['release_state']);
        }
        break;
    case 'package':
        include_once 'PEAR/Packager.php';
        $pkginfofile = $options[1][2];
        $packager =& new PEAR_Packager($script_dir, $ext_dir, $doc_dir);
        $packager->debug = $verbose;
        if (PEAR::isError($packager->Package($pkginfofile))) {
            print "\npackage failed\n";
        } else {
            print "package ok\n";
        }
        break;
    default:
        usage();
        break;
}

function usage($obj = null)
{
    $stderr = fopen('php://stderr', 'w');
    if ($obj !== null) {
        fputs($stderr, $obj->getMessage());
    }
    fputs($stderr,
          "Usage: pear [-v n] [-h] [-p <dir>] [-e <dir>] [-d <dir>] command <parameters>\n".
          "Options:\n".
          "     -v        set verbosity level to <n> (0-2, default 1)\n".
          "     -p <dir>  set script install dir (absolute path)\n".
          "     -e <dir>  set extension install dir (absolute path)\n".
          "     -d <dir>  set documentation dest dir (absolute path)\n".
          "     -h, -?    display help/usage (this message)\n".
          "Commands:\n".
          "   list-installed \n".
          "   install <package file>\n".
          "   uninstall <package file>\n".
          "   package [package info file]\n".
          "\n");
    fclose($stderr);
    exit;
}

?>