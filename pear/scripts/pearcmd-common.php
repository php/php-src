<?php

require_once "PEAR/Config.php";
require_once "Console/Getopt.php";

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

if (OS_WINDOWS) {
    $pear_default_config = PHP_SYSCONFDIR.DIRECTORY_SEPARATOR.'pearsys.ini';
    $pear_user_config    = PHP_SYSCONFDIR.DIRECTORY_SEPARATOR.'pear.ini';
} else {
    $pear_default_config = PHP_SYSCONFDIR.DIRECTORY_SEPARATOR.'pear.conf';
    $pear_user_config    = getenv('HOME').DIRECTORY_SEPARATOR.'.pearrc';
}

$opts = $options[0];

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
    'php_dir'       => PEAR_INSTALL_DIR,
    'ext_dir'       => PEAR_EXTENSION_DIR,
    'doc_dir'       => PHP_DATADIR . DIRECTORY_SEPARATOR . 'pear' .
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

// {{{ usage()

function usage($error = null, $helpsubject = null)
{
    global $progname, $subcommands;
    $stderr = fopen('php://stderr', 'w');
    if (PEAR::isError($error)) {
        fputs($stderr, $error->getMessage());
    } elseif ($error !== null) {
        fputs($stderr, $error);
    }
    fputs($stderr,
        "Usage: $progname [options] command [command-options] <parameters>\n");
    if ($helpsubject == "options") {
        fputs($stderr,
        "Options:\n".
        "     -v         increase verbosity level (default 1)\n".
        "     -q         be quiet, decrease verbosity level\n".
        "     -c file    find user configuration in `file'\n".
        "     -C file    find system configuration in `file'\n".
        "     -d foo=bar set user config variable `foo' to `bar'\n".
        "     -D foo=bar set system config variable `foo' to `bar'\n".
        "     -s         store user configuration\n".
        "     -S         store system configuration\n".
        "     -u foo     unset `foo' in the user configuration\n".
        "     -h, -?     display help/usage (this message)\n");
    } else {
        fputs($stderr, "Type \"$progname help options\" to list all options.\n");
    }        
    fputs($stderr, "Commands:".implode("\n   ", $subcommands) . "\n");
    fclose($stderr);
    exit;
}

// }}}
// {{{ present_array()

function present_array(&$arr, $keys = null)
{
    if ($keys === null) {
        $keys = array_keys($arr);
    }
    $longest_key = max(array_map("strlen", array_keys($arr))) + 2;
    $format_string = "%{$longest_key}s : %s\n";
    foreach ($keys as $k) {
        if (is_array($arr[$k])) {
            foreach ($arr[$k] as $i => $value) {
                $x = "$k #$i";
                $cont = array();
                foreach(array_keys($value) as $val) {
                    $cont[] = "$val=" . $value[$val];
                }
                $v = implode(", ", $cont);
                printf($format_string, $x, $v);
            }
            continue;
        } else {
            $v = $arr[$k];
            printf($format_string, $k, $v);
        }
    }
}

// }}}
// {{{ heading()

function heading($text)
{
    $l = strlen(trim($text));
    print rtrim($text) . "\n" . str_repeat("=", $l) . "\n";
}

// }}}

?>