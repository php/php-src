#!/usr/bin/env php
<?php declare(strict_types=1);

/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://php.net/license/3_01.txt                                     |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Peter Kokot <petk@php.net>                                  |
  +----------------------------------------------------------------------+
*/

/**
 * Usage and help information.
 */
function help(): string
{
    return <<<HELP
    NAME

        permissions.php - check php-src files for 0644 and 0755 permissions

    SYNOPSIS:

        php permissions.php [OPTION...] input-path

    DESCRIPTION:

        Git can track executable (0755) and non-executable (0644) file
        modes.

        This script checks and fixes file permissions in the php-src repository
        according to the predefined executable files with 0755 permissions and
        all others with 0644 permissions.

    OPTIONS:

        -h, --help       Display this help

            --no-colors  Disable colors in the script output

        -f, --fix        Change permissions according to the predefined settings

        -q, --quiet      Do not output any message

    EXAMPLES:

        Check php-src permissions: scripts/dev/permissions .

        Fix php-src permissions: scripts/dev/permissions.php -f .

    HELP;
}

/**
 * Return short invalid usage error.
 */
function invalidUsage(string $error): string
{
    $output = "permissions.php: $error\n";
    $output .= "Try 'php permissions.php --help' for more information.\n";

    return $output;
}

/**
 * Output message.
 */
function output(string $message, bool $quiet): void
{
    if ($quiet) {
        return;
    }

    echo $message."\n";
};

/**
 * Get filtered list of files from given path.
 */
function getFiles(array $opt): array
{
    if (is_file($opt['path'])) {
        return [$opt['path']];
    }

    $filter = function ($file, $key, $iterator) use ($opt) {
        $pathnameWithoutBase = str_replace($opt['path'].'/', '', $file->getPathname());

        return (!in_array($pathnameWithoutBase, $opt['ignores']));
    };

    $innerIterator = new \RecursiveDirectoryIterator(
        $opt['path'],
        \RecursiveDirectoryIterator::SKIP_DOTS
    );

    $iterator = new \RecursiveIteratorIterator(
        new \RecursiveCallbackFilterIterator($innerIterator, $filter)
    );

    $files = [];

    foreach ($iterator as $pathName => $fileInfo) {
        $files[] = $pathName;
    }

    return $files;
}

// Default configuration options
$opt = [
    'fix'           => false,
    'colors'        => true,
    'quiet'         => false,
    'color_red'     => "\e[31m",
    'color_green'   => "\e[32m",
    'color_default' => "\e[39m",
];

// Ignore the following directories and files
$opt['ignores'] = ['.git'];

# These files should have executable permissions
$opt['executables'] = [
    // *nix build files
    'build/buildcheck.sh',
    'build/config-stubs',
    'build/genif.sh',
    'build/shtool',
    'build/config.guess',
    'build/config.sub',
    'build/ltmain.sh',
    'buildconf',
    'genfiles',
    'makedist',

    // Extensions scripts
    'ext/curl/sync-constants.php',
    'ext/dba/install_cdb.sh',
    'ext/fileinfo/create_data_file.php',
    'ext/fileinfo/fileinfo.php',
    'ext/hash/bench.php',
    'ext/mbstring/libmbfl/filters/mk_emoji_tbl.pl',
    'ext/mbstring/libmbfl/filters/mk_sb_tbl.awk',
    'ext/mbstring/ucgendat/ucgendat.php',
    'ext/mbstring/ucgendat/uctest.php',
    'ext/pcntl/test-pcntl.php',
    'ext/pcre/upgrade-pcre.php',
    'ext/pdo/pdo.php',
    'ext/pdo_mysql/get_error_codes.php',
    'ext/phar/phar/phar.php',
    'ext/phar/build_precommand.php',
    'ext/session/mod_files.sh',
    'ext/snmp/tests/bigtest',
    'ext/standard/html_tables/html_table_gen.php',
    'ext/tokenizer/tokenizer_data_gen.sh',
    'ext/ext_skel.php',

    // SAPIs scripts
    'sapi/cli/generate_mime_type_map.php',
    'sapi/phpdbg/phpdbg.init.d',
    'sapi/phpdbg/create-test.php',

    // Internal common development scripts
    'scripts/dev/bless_tests.php',
    'scripts/dev/check_parameters.php',
    'scripts/dev/credits',
    'scripts/dev/find_tested.php',
    'scripts/dev/gen_verify_stub',
    'scripts/dev/permissions.php',
    'scripts/dev/phpextdist',
    'scripts/dev/search_underscores.php',
    'scripts/dev/snapshot',
    'scripts/dev/vcsclean',

    // Travis CI scripts
    'travis/ext/curl/setup.sh',
    'travis/ext/pdo_mysql/setup.sh',
    'travis/ext/pgsql/setup.sh',
    'travis/ext/mysql/setup.sh',
    'travis/ext/mysqli/setup.sh',
    'travis/ext/pdo_pgsql/setup.sh',
    'travis/compile.sh',

    // Zend development scripts
    'Zend/zend_vm_gen.php',

    // Script for running phpt tests
    'run-tests.php',

    // Windows related scripts
    'win32/build/registersyslog.php',
];

foreach ($argv as $argument) {
    switch ($argument) {
        case '-h':
        case '--help':
            echo help();
            exit;
        break;
        case '-f':
        case '--fix':
            $opt['fix'] = true;
        break;
        case '--no-colors':
            $opt['color_red'] = '';
            $opt['color_green'] = '';
            $opt['color_default'] = '';
        break;
        case '-q':
        case '--quiet':
            $opt['quiet'] = true;
        break;
        default:
            if ($argc < 2) {
                echo help();
                exit;
            }

            if ((preg_match('/^-.+/', $argument))) {
                echo invalidUsage("invalid option '$argument'");
                exit(1);
            }

            if (file_exists($argument) && $argument !== $argv[0]) {
                $opt['path'] = $argument;
            }
        break;
    }
}

if (empty($opt['path'])) {
    echo invalidUsage('missing path');
    exit(1);
}

// Check if path is the root of the php-src repository
if (!array_filter([$opt['path'].'/build', $opt['path'].'/ext', $opt['path'].'/main/php.h', $opt['path'].'/Zend'], 'file_exists')) {
    echo invalidUsage('path needs to be php-src repository');
    exit(1);
}

// Check if all above defined executable files exist in the php-src
foreach ($opt['executables'] as $exe) {
    if (!file_exists($opt['path'].'/'.$exe)) {
        output($opt['path'].'/'.$exe.': '.$opt['color_red'].'file not found'.$opt['color_default'], $opt['quiet']);
    }
}

output('Executing '.$opt['color_green'].'permissions.php'.$opt['color_default']."\n", $opt['quiet']);

$exitCode = 0;

foreach (getFiles($opt) as $file) {
    // Get permissions numeric value 0755 or 0644
    $permissions = $newPermissions = (fileperms($file) & 0777);

    if (in_array(str_replace($opt['path'].'/', '', $file), $opt['executables'])) {
        // File should be executable
        $newPermissions = (!is_executable($file)) ? 0755 : $permissions;
    } elseif (is_executable($file)) {
        // File shouldn't be executable
        $newPermissions = 0644;
    } elseif ($permissions !== 0644 && $permissions !== 0755) {
        // Unknown permissions, set back to default 0644
        $newPermissions = 0644;
    }

    if ($permissions !== $newPermissions) {
        if ($opt['fix']) {
            chmod($file, $newPermissions);
            output($file.': '.$opt['color_green'].'permissions fixed from '.decoct($permissions).' to '.decoct($newPermissions).$opt['color_default'], $opt['quiet']);
            $exitCode = 0;
        } else {
            output($file.': '.$opt['color_red'].'permissions '.decoct($permissions).$opt['color_default'], $opt['quiet']);
            $exitCode = 1;
        }
    }
}

exit($exitCode);
