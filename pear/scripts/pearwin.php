<?php
//
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2003 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Stig Bakken <ssb@php.net>                                   |
// |          Tomas V.V.Cox <cox@idecnet.com>                             |
// +----------------------------------------------------------------------+
//
// $Id$

require_once 'PEAR.php';
require_once 'PEAR/Common.php';
require_once 'PEAR/Config.php';
require_once 'PEAR/Remote.php';
require_once 'PEAR/Registry.php';
require_once 'Console/Getopt.php';

error_reporting(E_ALL ^ E_NOTICE);

$progname = basename($argv[0]);

PEAR::setErrorHandling(PEAR_ERROR_PRINT, "$progname: %s\n");

$argv = Console_Getopt::readPHPArgv();
if (PEAR::isError($argv)) {
    die($argv->getMessage());
}
$options = Console_Getopt::getopt($argv, "c:C:d:D:h?sSqu:v");
if (PEAR::isError($options)) {
    usage($options);
}


$php_sysconfdir = getenv('PHP_SYSCONFDIR');
if (!empty($php_sysconfdir)) {
    $pear_default_config = $php_sysconfdir.DIRECTORY_SEPARATOR.'pearsys.ini';
    $pear_user_config    = $php_sysconfdir.DIRECTORY_SEPARATOR.'pear.ini';
} else {
    $pear_default_config = PHP_SYSCONFDIR.DIRECTORY_SEPARATOR.'pearsys.ini';
    $pear_user_config    = PHP_SYSCONFDIR.DIRECTORY_SEPARATOR.'pear.ini';
}

$opts = $options[0];

//echo "ini_get : ".ini_get("pear_install_dir")."\n";
//echo "get_cfg_var : ".get_cfg_var("pear_install_dir")."\n";

foreach ($opts as $opt) {
    switch ($opt[0]) {
        case 'c':
            $pear_user_config = $opt[1];
            break;
        case 'C':
            $pear_default_config = $opt[1];
            break;
    }
}

$config = new PEAR_Config($pear_user_config, $pear_default_config);
$store_user_config = false;
$store_default_config = false;
$verbose = 1;

foreach ($opts as $opt) {
    $param = $opt[1];
    switch ($opt[0]) {
        case 'd':
            list($key, $value) = explode('=', $param);
            $config->set($key, $value);
            break;
        case 'D':
            list($key, $value) = explode('=', $param);
            $config->set($key, $value, true);
            break;
        case 's':
            $store_user_config = true;
            break;
        case 'S':
            $store_default_config = true;
            break;
        case 'u':
            $config->toDefault($param);
            break;
        case 'v':
            $verbose++;
            break;
        case 'q':
            $verbose--;
            break;
    }
}

if ($store_default_config) {
    if (@is_writeable($pear_default_config)) {
        $config->writeConfigFile($pear_default_config, 'default');
    } else {
        die("You don't have write access to $pear_default_config, exiting!\n");
    }
}

if ($store_user_config) {
    $config->writeConfigFile($pear_user_config, 'userdefined');
}

$fallback_config = array(
    'master_server' => 'pear.php.net',
    'php_dir'       => getenv('PEAR_INSTALL_DIR'),
    'ext_dir'       => getenv('PEAR_EXTENSION_DIR'),
    'doc_dir'       => getenv('PHP_DATADIR') . DIRECTORY_SEPARATOR . 'pear' .
                       DIRECTORY_SEPARATOR . 'doc',
    'verbose'       => true,
);
$fallback_done = array();

foreach ($fallback_config as $key => $value) {
    if (!$config->isDefined($key)) {
        $config->set($key, $value);
        $fallback_done[$key] = true;
    }
}

//$verbose    = $config->get("verbose");
$script_dir = $config->get("php_dir");
$ext_dir    = $config->get("ext_dir");
$doc_dir    = $config->get("doc_dir");

PEAR::setErrorHandling(PEAR_ERROR_PRINT);

$command = (isset($options[1][1])) ? $options[1][1] : null;
$rest = array_slice($options[1], 2);

if (isset($command_options[$command])) {
    $tmp = Console_Getopt::getopt($rest, $command_options[$command]);
    if (PEAR::isError($tmp)) {
        usage($tmp);
    }
    $cmdopts = $tmp[0];
    $cmdargs = $tmp[1];
} else {
    $cmdopts = array();
    $cmdargs = $rest;
}


/* Extracted from pearcmd-common.php */
function heading($text)
{
    $l = strlen(trim($text));
    print rtrim($text) . "\n" . str_repeat("=", $l) . "\n";
}

switch ($command) {
    case 'install':
        include 'pearcmd-install.php';
        break;
    case 'uninstall':
        include 'pearcmd-uninstall.php';
        break;
    case 'list':
        include 'pearcmd-list.php';
        break;
    case 'package':
        include 'pearcmd-package.php';
        break;
    case 'remote-list':
        include 'pearcmd-remote-list.php';
        break;
    case 'show-config':
        $keys = $config->getKeys();
        foreach ($keys as $key) {
            $value = $config->get($key);
            $xi = "";
            if ($config->isDefaulted($key)) {
                $xi .= " (default)";
            }
            if (isset($fallback_done[$key])) {
                $xi .= " (built-in)";
            }
            printf("%s = %s%s\n", $key, $value, $xi);
        }
        break;
    default: {
        if (!$store_default_config && !$store_user_config) {
            usage();
        }
        break;
    }
}

function usage($obj = null)
{
    $stderr = fopen('php://stderr', 'w');
    if ($obj !== null) {
        fputs($stderr, $obj->getMessage());
    }
    fputs($stderr,
          "Usage: pear [options] command [command-options] <parameters>\n".
          "Options:\n".
          "     -v             increase verbosity level (default 1)\n".
          "     -q             be quiet, decrease verbosity level\n".
          "     -c file        find user configuration in `file'\n".
          "     -C file        find system configuration in `file'\n".
          "     -d \"foo=bar\" set user config variable `foo' to `bar'\n".
          "     -D \"foo=bar\" set system config variable `foo' to `bar'\n".
          "     -s             store user configuration\n".
          "     -S             store system configuration\n".
          "     -u foo         unset `foo' in the user configuration\n".
          "     -h, -?         display help/usage (this message)\n".
          "Commands:\n".
          "   help [command]\n".
          "   install [-r] <package file>\n".
          "   uninstall [-r] <package name>\n".
          "   package [package info file]\n".
          "   list\n".
          "   remote-list\n".
          "   show-config\n".
          "\n");
    fclose($stderr);
    exit;
}

?>