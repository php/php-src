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
// |          Tomas V.V.Cox <cox@idecnet.com>                             |
// |                                                                      |
// +----------------------------------------------------------------------+
//
// $Id$

ob_end_clean();
if (!defined('PEAR_RUNTYPE')) {
    // this is defined in peclcmd.php as 'pecl'
    define('PEAR_RUNTYPE', 'pear');
}
define('PEAR_IGNORE_BACKTRACE', 1);
/**
 * @nodep Gtk
 */
if ('@include_path@' != '@'.'include_path'.'@') {
    ini_set('include_path', '@include_path@');
    $raw = false;
} else {
    // this is a raw, uninstalled pear, either a cvs checkout, or php distro
    $raw = true;
}
@ini_set('allow_url_fopen', true);
if (!ini_get('safe_mode')) {
    @set_time_limit(0);
}
ob_implicit_flush(true);
@ini_set('track_errors', true);
@ini_set('html_errors', false);
@ini_set('magic_quotes_runtime', false);
$_PEAR_PHPDIR = '#$%^&*';
set_error_handler('error_handler');

$pear_package_version = "@pear_version@";

require_once 'PEAR.php';
require_once 'PEAR/Frontend.php';
require_once 'PEAR/Config.php';
require_once 'PEAR/Command.php';
require_once 'Console/Getopt.php';


PEAR_Command::setFrontendType('CLI');
$all_commands = PEAR_Command::getCommands();

$argv = Console_Getopt::readPHPArgv();
$progname = PEAR_RUNTYPE;
if (in_array('getopt2', get_class_methods('Console_Getopt'))) {
    array_shift($argv);
    $options = Console_Getopt::getopt2($argv, "c:C:d:D:Gh?sSqu:vV");
} else {
    $options = Console_Getopt::getopt($argv, "c:C:d:D:Gh?sSqu:vV");
}
if (PEAR::isError($options)) {
    usage($options);
}

$opts = $options[0];

$fetype = 'CLI';
if ($progname == 'gpear' || $progname == 'pear-gtk') {
    $fetype = 'Gtk';
} else {
    foreach ($opts as $opt) {
        if ($opt[0] == 'G') {
            $fetype = 'Gtk';
        }
    }
}
$pear_user_config = '';
$pear_system_config = '';
$store_user_config = false;
$store_system_config = false;
$verbose = 1;

foreach ($opts as $opt) {
    switch ($opt[0]) {
        case 'c':
            $pear_user_config = $opt[1];
            break;
        case 'C':
            $pear_system_config = $opt[1];
            break;
    }
}

PEAR_Command::setFrontendType($fetype);
$ui = &PEAR_Command::getFrontendObject();
$config = &PEAR_Config::singleton($pear_user_config, $pear_system_config);

if (PEAR::isError($config)) {
    $_file = '';
    if ($pear_user_config !== false) {
       $_file .= $pear_user_config;
    }
    if ($pear_system_config !== false) {
       $_file .= '/' . $pear_system_config;
    }
    if ($_file == '/') {
        $_file = 'The default config file';
    }
    $config->getMessage();
    $ui->outputData("ERROR: $_file is not a valid config file or is corrupted.");
    // We stop, we have no idea where we are :)
    exit();    
}

// this is used in the error handler to retrieve a relative path
$_PEAR_PHPDIR = $config->get('php_dir');
$ui->setConfig($config);
PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, array($ui, "displayFatalError"));
if (ini_get('safe_mode')) {
    $ui->outputData('WARNING: running in safe mode requires that all files created ' .
        'be the same uid as the current script.  PHP reports this script is uid: ' .
        @getmyuid() . ', and current user is: ' . @get_current_user());
}

$verbose = $config->get("verbose");
$cmdopts = array();

if ($raw) {
    if (!$config->isDefinedLayer('user') && !$config->isDefinedLayer('system')) {
        $found = false;
        foreach ($opts as $opt) {
            if ($opt[0] == 'd' || $opt[0] == 'D') {
                $found = true; // the user knows what they are doing, and are setting config values
            }
        }
        if (!$found) {
            // no prior runs, try to install PEAR
            if (strpos(dirname(__FILE__), 'scripts')) {
                $packagexml = dirname(dirname(__FILE__)) . DIRECTORY_SEPARATOR . 'package2.xml';
                $pearbase = dirname(dirname(__FILE__));
            } else {
                $packagexml = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'package2.xml';
                $pearbase = dirname(__FILE__);
            }
            if (file_exists($packagexml)) {
                $options[1] = array(
                    'install',
                    $packagexml
                );
                $config->set('php_dir', $pearbase . DIRECTORY_SEPARATOR . 'php');
                $config->set('data_dir', $pearbase . DIRECTORY_SEPARATOR . 'data');
                $config->set('doc_dir', $pearbase . DIRECTORY_SEPARATOR . 'docs');
                $config->set('test_dir', $pearbase . DIRECTORY_SEPARATOR . 'tests');
                $config->set('ext_dir', $pearbase . DIRECTORY_SEPARATOR . 'extensions');
                $config->set('bin_dir', $pearbase);
                $config->mergeConfigFile($pearbase . 'pear.ini', false);
                $config->store();
                $config->set('auto_discover', 1);
            }
        }
    }
}
foreach ($opts as $opt) {
    $param = !empty($opt[1]) ? $opt[1] : true;
    switch ($opt[0]) {
        case 'd':
            list($key, $value) = explode('=', $param);
            $config->set($key, $value, 'user');
            break;
        case 'D':
            list($key, $value) = explode('=', $param);
            $config->set($key, $value, 'system');
            break;
        case 's':
            $store_user_config = true;
            break;
        case 'S':
            $store_system_config = true;
            break;
        case 'u':
            $config->remove($param, 'user');
            break;
        case 'v':
            $config->set('verbose', $config->get('verbose') + 1);
            break;
        case 'q':
            $config->set('verbose', $config->get('verbose') - 1);
            break;
        case 'V':
            usage(null, 'version');
        case 'c':
        case 'C':
            break;
        default:
            // all non pear params goes to the command
            $cmdopts[$opt[0]] = $param;
            break;
    }
}

if ($store_system_config) {
    $config->store('system');
}

if ($store_user_config) {
    $config->store('user');
}

$command = (isset($options[1][0])) ? $options[1][0] : null;

if (empty($command) && ($store_user_config || $store_system_config)) {
    exit;
}

if ($fetype == 'Gtk') {
    if (!$config->validConfiguration()) {
        PEAR::raiseError('CRITICAL ERROR: no existing valid configuration files found in files ' .
            "'$pear_user_config' or '$pear_system_config', please copy an existing configuration" .
            'file to one of these locations, or use the -c and -s options to create one');
    }
    Gtk::main();
} else do {
    if ($command == 'help') {
        usage(null, @$options[1][1]);
    }
    if (!$config->validConfiguration()) {
        PEAR::raiseError('CRITICAL ERROR: no existing valid configuration files found in files ' .
            "'$pear_user_config' or '$pear_system_config', please copy an existing configuration" .
            'file to one of these locations, or use the -c and -s options to create one');
    }

    PEAR::pushErrorHandling(PEAR_ERROR_RETURN);
    $cmd = PEAR_Command::factory($command, $config);
    PEAR::popErrorHandling();
    if (PEAR::isError($cmd)) {
        usage(null, @$options[1][0]);
    }

    $short_args = $long_args = null;
    PEAR_Command::getGetoptArgs($command, $short_args, $long_args);
    if (in_array('getopt2', get_class_methods('Console_Getopt'))) {
        array_shift($options[1]);
        $tmp = Console_Getopt::getopt2($options[1], $short_args, $long_args);
    } else {
        $tmp = Console_Getopt::getopt($options[1], $short_args, $long_args);
    }
    if (PEAR::isError($tmp)) {
        break;
    }
    list($tmpopt, $params) = $tmp;
    $opts = array();
    foreach ($tmpopt as $foo => $tmp2) {
        list($opt, $value) = $tmp2;
        if ($value === null) {
            $value = true; // options without args
        }
        if (strlen($opt) == 1) {
            $cmdoptions = $cmd->getOptions($command);
            foreach ($cmdoptions as $o => $d) {
                if (@$d['shortopt'] == $opt) {
                    $opts[$o] = $value;
                }
            }
        } else {
            if (substr($opt, 0, 2) == '--') {
                $opts[substr($opt, 2)] = $value;
            }
        }
    }
    $ok = $cmd->run($command, $opts, $params);
    if ($ok === false) {
        PEAR::raiseError("unknown command `$command'");
    }
    if (PEAR::isError($ok)) {
        PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, array($ui, "displayFatalError"));
        PEAR::raiseError($ok);
    }
} while (false);

// {{{ usage()

function usage($error = null, $helpsubject = null)
{
    global $progname, $all_commands;
    $stderr = fopen('php://stderr', 'w');
    if (PEAR::isError($error)) {
        fputs($stderr, $error->getMessage() . "\n");
    } elseif ($error !== null) {
        fputs($stderr, "$error\n");
    }
    if ($helpsubject != null) {
        $put = cmdHelp($helpsubject);
    } else {
        $put =
            "Commands:\n";
        $maxlen = max(array_map("strlen", $all_commands));
        $formatstr = "%-{$maxlen}s  %s\n";
        ksort($all_commands);
        foreach ($all_commands as $cmd => $class) {
            $put .= sprintf($formatstr, $cmd, PEAR_Command::getDescription($cmd));
        }
        $put .=
            "Usage: $progname [options] command [command-options] <parameters>\n".
            "Type \"$progname help options\" to list all options.\n".
            "Type \"$progname help shortcuts\" to list all command shortcuts.\n".
            "Type \"$progname help <command>\" to get the help for the specified command.";
    }
    fputs($stderr, "$put\n");
    fclose($stderr);
    exit;
}

function cmdHelp($command)
{
    global $progname, $all_commands, $config;
    if ($command == "options") {
        return
        "Options:\n".
        "     -v         increase verbosity level (default 1)\n".
        "     -q         be quiet, decrease verbosity level\n".
        "     -c file    find user configuration in `file'\n".
        "     -C file    find system configuration in `file'\n".
        "     -d foo=bar set user config variable `foo' to `bar'\n".
        "     -D foo=bar set system config variable `foo' to `bar'\n".
        "     -G         start in graphical (Gtk) mode\n".
        "     -s         store user configuration\n".
        "     -S         store system configuration\n".
        "     -u foo     unset `foo' in the user configuration\n".
        "     -h, -?     display help/usage (this message)\n".
        "     -V         version information\n";
    } elseif ($command == "shortcuts") {
        $sc = PEAR_Command::getShortcuts();
        $ret = "Shortcuts:\n";
        foreach ($sc as $s => $c) {
            $ret .= sprintf("     %-8s %s\n", $s, $c);
        }
        return $ret;

    } elseif ($command == "version") {
        return "PEAR Version: ".$GLOBALS['pear_package_version'].
               "\nPHP Version: ".phpversion().
               "\nZend Engine Version: ".zend_version().
               "\nRunning on: ".php_uname();

    } elseif ($help = PEAR_Command::getHelp($command)) {
        if (is_string($help)) {
            return "$progname $command [options] $help\n";
        }
        if ($help[1] === null) {
            return "$progname $command $help[0]";
        } else {
            return "$progname $command [options] $help[0]\n$help[1]";
        }
    }
    return "Command '$command' is not valid, try 'pear help'";
}

// }}}

function error_handler($errno, $errmsg, $file, $line, $vars) {
    if ((defined('E_STRICT') && $errno & E_STRICT) || !error_reporting()) {
        if (defined('E_STRICT') && $errno & E_STRICT) {
            return; // E_STRICT
        }
        if ($GLOBALS['config']->get('verbose') < 4) {
            return; // @silenced error, show all if debug is high enough
        }
    }
    $errortype = array (
        E_ERROR   =>  "Error",
        E_WARNING   =>  "Warning",
        E_PARSE   =>  "Parsing Error",
        E_NOTICE   =>  "Notice",
        E_CORE_ERROR  =>  "Core Error",
        E_CORE_WARNING  =>  "Core Warning",
        E_COMPILE_ERROR  =>  "Compile Error",
        E_COMPILE_WARNING =>  "Compile Warning",
        E_USER_ERROR =>  "User Error",
        E_USER_WARNING =>  "User Warning",
        E_USER_NOTICE =>  "User Notice"
    );
    $prefix = $errortype[$errno];
    global $_PEAR_PHPDIR;
    if (stristr($file, $_PEAR_PHPDIR)) {
        $file = substr($file, strlen($_PEAR_PHPDIR) + 1);
    } else {
        $file = basename($file);
    }
    print "\n$prefix: $errmsg in $file on line $line\n";
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * mode: php
 * End:
 */
// vim600:syn=php

?>
