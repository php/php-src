#!/usr/bin/env php
<?php
/*
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
   | Authors: Ilia Alshanetsky <iliaa@php.net>                            |
   |          Preston L. Bannister <pbannister@php.net>                   |
   |          Marcus Boerger <helly@php.net>                              |
   |          Derick Rethans <derick@php.net>                             |
   |          Sander Roobol <sander@php.net>                              |
   |          Andrea Faulds <ajf@ajf.me>                                  |
   | (based on version by: Stig Bakken <ssb@php.net>)                     |
   | (based on the PHP 3 test framework by Rasmus Lerdorf)                |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

/* Let there be no top-level code beyond this point:
 * Only functions and classes, thanks!
 *
 * Minimum required PHP version: 7.1.0
 */

function show_usage(): void
{
    echo <<<HELP
Synopsis:
    php run-tests.php [options] [files] [directories]

Options:
    -j<workers> Run up to <workers> simultaneous testing processes in parallel for
                quicker testing on systems with multiple logical processors.
                Note that this is experimental feature.

    -l <file>   Read the testfiles to be executed from <file>. After the test
                has finished all failed tests are written to the same <file>.
                If the list is empty and no further test is specified then
                all tests are executed (same as: -r <file> -w <file>).

    -r <file>   Read the testfiles to be executed from <file>.

    -w <file>   Write a list of all failed tests to <file>.

    -a <file>   Same as -w but append rather then truncating <file>.

    -W <file>   Write a list of all tests and their result status to <file>.

    -c <file>   Look for php.ini in directory <file> or use <file> as ini.

    -n          Pass -n option to the php binary (Do not use a php.ini).

    -d foo=bar  Pass -d option to the php binary (Define INI entry foo
                with value 'bar').

    -g          Comma separated list of groups to show during test run
                (possible values: PASS, FAIL, XFAIL, XLEAK, SKIP, BORK, WARN, LEAK, REDIRECT).

    -m          Test for memory leaks with Valgrind (equivalent to -M memcheck).

    -M <tool>   Test for errors with Valgrind tool.

    -p <php>    Specify PHP executable to run.

    -P          Use PHP_BINARY as PHP executable to run (default).

    -q          Quiet, no user interaction (same as environment NO_INTERACTION).

    -s <file>   Write output to <file>.

    -x          Sets 'SKIP_SLOW_TESTS' environmental variable.

    --offline   Sets 'SKIP_ONLINE_TESTS' environmental variable.

    --verbose
    -v          Verbose mode.

    --help
    -h          This Help.

    --temp-source <sdir>  --temp-target <tdir> [--temp-urlbase <url>]
                Write temporary files to <tdir> by replacing <sdir> from the
                filenames to generate with <tdir>. In general you want to make
                <sdir> the path to your source files and <tdir> some patch in
                your web page hierarchy with <url> pointing to <tdir>.

    --keep-[all|php|skip|clean]
                Do not delete 'all' files, 'php' test file, 'skip' or 'clean'
                file.

    --set-timeout <n>
                Set timeout for individual tests, where <n> is the number of
                seconds. The default value is 60 seconds, or 300 seconds when
                testing for memory leaks.

    --context <n>
                Sets the number of lines of surrounding context to print for diffs.
                The default value is 3.

    --show-[all|php|skip|clean|exp|diff|out|mem]
                Show 'all' files, 'php' test file, 'skip' or 'clean' file. You
                can also use this to show the output 'out', the expected result
                'exp', the difference between them 'diff' or the valgrind log
                'mem'. The result types get written independent of the log format,
                however 'diff' only exists when a test fails.

    --show-slow <n>
                Show all tests that took longer than <n> milliseconds to run.

    --no-clean  Do not execute clean section if any.

    --color
    --no-color  Do/Don't colorize the result type in the test result.


HELP;
}

/**
 * One function to rule them all, one function to find them, one function to
 * bring them all and in the darkness bind them.
 * This is the entry point and exit point überfunction. It contains all the
 * code that was previously found at the top level. It could and should be
 * refactored to be smaller and more manageable.
 */
function main(): void
{
    /* This list was derived in a naïve mechanical fashion. If a member
     * looks like it doesn't belong, it probably doesn't; cull at will.
     */
    global $DETAILED, $PHP_FAILED_TESTS, $SHOW_ONLY_GROUPS, $argc, $argv, $cfg,
           $cfgfiles, $cfgtypes, $conf_passed, $end_time, $environment,
           $exts_skipped, $exts_tested, $exts_to_test, $failed_tests_file,
           $ignored_by_ext, $ini_overwrites, $is_switch, $colorize,
           $just_save_results, $log_format, $matches, $no_clean, $no_file_cache,
           $optionals, $output_file, $pass_option_n, $pass_options,
           $pattern_match, $php, $php_cgi, $phpdbg, $preload, $redir_tests,
           $repeat, $result_tests_file, $slow_min_ms, $start_time, $switch,
           $temp_source, $temp_target, $test_cnt, $test_dirs,
           $test_files, $test_idx, $test_list, $test_results, $testfile,
           $user_tests, $valgrind, $sum_results, $shuffle, $file_cache;
    // Parallel testing
    global $workers, $workerID;
    global $context_line_count;

    define('IS_WINDOWS', substr(PHP_OS, 0, 3) == "WIN");

    $workerID = 0;
    if (getenv("TEST_PHP_WORKER")) {
        $workerID = intval(getenv("TEST_PHP_WORKER"));
        run_worker();
        return;
    }

    define('INIT_DIR', getcwd());

    // Change into the PHP source directory.
    if (getenv('TEST_PHP_SRCDIR')) {
        @chdir(getenv('TEST_PHP_SRCDIR'));
    }

    define('TEST_PHP_SRCDIR', getcwd());

    check_proc_open_function_exists();

    // If timezone is not set, use UTC.
    if (ini_get('date.timezone') == '') {
        date_default_timezone_set('UTC');
    }

    // Delete some security related environment variables
    putenv('SSH_CLIENT=deleted');
    putenv('SSH_AUTH_SOCK=deleted');
    putenv('SSH_TTY=deleted');
    putenv('SSH_CONNECTION=deleted');

    set_time_limit(0);

    ini_set('pcre.backtrack_limit', PHP_INT_MAX);

    init_output_buffers();

    error_reporting(E_ALL);

    $environment = $_ENV ?? [];

    // Some configurations like php.ini-development set variables_order="GPCS"
    // not "EGPCS", in which case $_ENV is NOT populated. Detect if the $_ENV
    // was empty and handle it by explicitly populating through getenv().
    if (empty($environment)) {
        $environment = getenv();
    }

    if (empty($environment['TEMP'])) {
        $environment['TEMP'] = sys_get_temp_dir();

        if (empty($environment['TEMP'])) {
            // For example, OpCache on Windows will fail in this case because
            // child processes (for tests) will not get a TEMP variable, so
            // GetTempPath() will fallback to c:\windows, while GetTempPath()
            // will return %TEMP% for parent (likely a different path). The
            // parent will initialize the OpCache in that path, and child will
            // fail to reattach to the OpCache because it will be using the
            // wrong path.
            die("TEMP environment is NOT set");
        } else {
            if (count($environment) == 1) {
                // Not having other environment variables, only having TEMP, is
                // probably ok, but strange and may make a difference in the
                // test pass rate, so warn the user.
                echo "WARNING: Only 1 environment variable will be available to tests(TEMP environment variable)" . PHP_EOL;
            }
        }
    }

    if (IS_WINDOWS && empty($environment["SystemRoot"])) {
        $environment["SystemRoot"] = getenv("SystemRoot");
    }

    $php = null;
    $php_cgi = null;
    $phpdbg = null;

    if (getenv('TEST_PHP_EXECUTABLE')) {
        $php = getenv('TEST_PHP_EXECUTABLE');

        if ($php == 'auto') {
            $php = TEST_PHP_SRCDIR . '/sapi/cli/php';
            putenv("TEST_PHP_EXECUTABLE=$php");

            if (!getenv('TEST_PHP_CGI_EXECUTABLE')) {
                $php_cgi = TEST_PHP_SRCDIR . '/sapi/cgi/php-cgi';

                if (file_exists($php_cgi)) {
                    putenv("TEST_PHP_CGI_EXECUTABLE=$php_cgi");
                } else {
                    $php_cgi = null;
                }
            }
        }
        $environment['TEST_PHP_EXECUTABLE'] = $php;
    }

    if (getenv('TEST_PHP_CGI_EXECUTABLE')) {
        $php_cgi = getenv('TEST_PHP_CGI_EXECUTABLE');

        if ($php_cgi == 'auto') {
            $php_cgi = TEST_PHP_SRCDIR . '/sapi/cgi/php-cgi';
            putenv("TEST_PHP_CGI_EXECUTABLE=$php_cgi");
        }

        $environment['TEST_PHP_CGI_EXECUTABLE'] = $php_cgi;
    }

    if (!getenv('TEST_PHPDBG_EXECUTABLE')) {
        if (IS_WINDOWS && file_exists(dirname($php) . "/phpdbg.exe")) {
            $phpdbg = realpath(dirname($php) . "/phpdbg.exe");
        } elseif (file_exists(dirname($php) . "/../../sapi/phpdbg/phpdbg")) {
            $phpdbg = realpath(dirname($php) . "/../../sapi/phpdbg/phpdbg");
        } elseif (file_exists("./sapi/phpdbg/phpdbg")) {
            $phpdbg = realpath("./sapi/phpdbg/phpdbg");
        } elseif (file_exists(dirname($php) . "/phpdbg")) {
            $phpdbg = realpath(dirname($php) . "/phpdbg");
        } else {
            $phpdbg = null;
        }
        if ($phpdbg) {
            putenv("TEST_PHPDBG_EXECUTABLE=$phpdbg");
        }
    }

    if (getenv('TEST_PHPDBG_EXECUTABLE')) {
        $phpdbg = getenv('TEST_PHPDBG_EXECUTABLE');

        if ($phpdbg == 'auto') {
            $phpdbg = TEST_PHP_SRCDIR . '/sapi/phpdbg/phpdbg';
            putenv("TEST_PHPDBG_EXECUTABLE=$phpdbg");
        }

        $environment['TEST_PHPDBG_EXECUTABLE'] = $phpdbg;
    }

    if (getenv('TEST_PHP_LOG_FORMAT')) {
        $log_format = strtoupper(getenv('TEST_PHP_LOG_FORMAT'));
    } else {
        $log_format = 'LEODS';
    }

    // Check whether a detailed log is wanted.
    if (getenv('TEST_PHP_DETAILED')) {
        $DETAILED = getenv('TEST_PHP_DETAILED');
    } else {
        $DETAILED = 0;
    }

    junit_init();

    if (getenv('SHOW_ONLY_GROUPS')) {
        $SHOW_ONLY_GROUPS = explode(",", getenv('SHOW_ONLY_GROUPS'));
    } else {
        $SHOW_ONLY_GROUPS = [];
    }

    // Check whether user test dirs are requested.
    if (getenv('TEST_PHP_USER')) {
        $user_tests = explode(',', getenv('TEST_PHP_USER'));
    } else {
        $user_tests = [];
    }

    $exts_to_test = [];
    $ini_overwrites = [
        'output_handler=',
        'open_basedir=',
        'disable_functions=',
        'output_buffering=Off',
        'error_reporting=' . E_ALL,
        'display_errors=1',
        'display_startup_errors=1',
        'log_errors=0',
        'html_errors=0',
        'track_errors=0',
        'report_memleaks=1',
        'report_zend_debug=0',
        'docref_root=',
        'docref_ext=.html',
        'error_prepend_string=',
        'error_append_string=',
        'auto_prepend_file=',
        'auto_append_file=',
        'ignore_repeated_errors=0',
        'precision=14',
        'serialize_precision=-1',
        'memory_limit=128M',
        'log_errors_max_len=0',
        'opcache.fast_shutdown=0',
        'opcache.file_update_protection=0',
        'opcache.revalidate_freq=0',
        'opcache.jit_hot_loop=1',
        'opcache.jit_hot_func=1',
        'opcache.jit_hot_return=1',
        'opcache.jit_hot_side_exit=1',
        'zend.assertions=1',
        'zend.exception_ignore_args=0',
        'zend.exception_string_param_max_len=15',
        'short_open_tag=0',
    ];

    $no_file_cache = '-d opcache.file_cache= -d opcache.file_cache_only=0';

    define('PHP_QA_EMAIL', 'qa-reports@lists.php.net');
    define('QA_SUBMISSION_PAGE', 'http://qa.php.net/buildtest-process.php');
    define('QA_REPORTS_PAGE', 'http://qa.php.net/reports');
    define('TRAVIS_CI', (bool) getenv('TRAVIS'));

    // Determine the tests to be run.

    $test_files = [];
    $redir_tests = [];
    $test_results = [];
    $PHP_FAILED_TESTS = [
        'BORKED' => [],
        'FAILED' => [],
        'WARNED' => [],
        'LEAKED' => [],
        'XFAILED' => [],
        'XLEAKED' => [],
        'SLOW' => []
    ];

    // If parameters given assume they represent selected tests to run.
    $result_tests_file = false;
    $failed_tests_file = false;
    $pass_option_n = false;
    $pass_options = '';

    $output_file = INIT_DIR . '/php_test_results_' . date('Ymd_Hi') . '.txt';

    $just_save_results = false;
    $valgrind = null;
    $temp_source = null;
    $temp_target = null;
    $conf_passed = null;
    $no_clean = false;
    $colorize = true;
    if (function_exists('sapi_windows_vt100_support') && !sapi_windows_vt100_support(STDOUT, true)) {
        $colorize = false;
    }
    if (array_key_exists('NO_COLOR', $_ENV)) {
        $colorize = false;
    }
    $selected_tests = false;
    $slow_min_ms = INF;
    $preload = false;
    $file_cache = null;
    $shuffle = false;
    $workers = null;
    $context_line_count = 3;

    $cfgtypes = ['show', 'keep'];
    $cfgfiles = ['skip', 'php', 'clean', 'out', 'diff', 'exp', 'mem'];
    $cfg = [];

    foreach ($cfgtypes as $type) {
        $cfg[$type] = [];

        foreach ($cfgfiles as $file) {
            $cfg[$type][$file] = false;
        }
    }

    if (!isset($argc, $argv) || !$argc) {
        $argv = [__FILE__];
        $argc = 1;
    }

    if (getenv('TEST_PHP_ARGS')) {
        $argv = array_merge($argv, explode(' ', getenv('TEST_PHP_ARGS')));
        $argc = count($argv);
    }

    for ($i = 1; $i < $argc; $i++) {
        $is_switch = false;
        $switch = substr($argv[$i], 1, 1);
        $repeat = substr($argv[$i], 0, 1) == '-';

        while ($repeat) {
            if (!$is_switch) {
                $switch = substr($argv[$i], 1, 1);
            }

            $is_switch = true;

            if ($repeat) {
                foreach ($cfgtypes as $type) {
                    if (strpos($switch, '--' . $type) === 0) {
                        foreach ($cfgfiles as $file) {
                            if ($switch == '--' . $type . '-' . $file) {
                                $cfg[$type][$file] = true;
                                $is_switch = false;
                                break;
                            }
                        }
                    }
                }
            }

            if (!$is_switch) {
                $is_switch = true;
                break;
            }

            $repeat = false;

            switch ($switch) {
                case 'j':
                    $workers = substr($argv[$i], 2);
                    if (!preg_match('/^\d+$/', $workers) || $workers == 0) {
                        error("'$workers' is not a valid number of workers, try e.g. -j16 for 16 workers");
                    }
                    $workers = intval($workers, 10);
                    // Don't use parallel testing infrastructure if there is only one worker.
                    if ($workers === 1) {
                        $workers = null;
                    }
                    break;
                case 'r':
                case 'l':
                    $test_list = file($argv[++$i]);
                    if ($test_list) {
                        foreach ($test_list as $test) {
                            $matches = [];
                            if (preg_match('/^#.*\[(.*)\]\:\s+(.*)$/', $test, $matches)) {
                                $redir_tests[] = [$matches[1], $matches[2]];
                            } else {
                                if (strlen($test)) {
                                    $test_files[] = trim($test);
                                }
                            }
                        }
                    }
                    if ($switch != 'l') {
                        break;
                    }
                    $i--;
                // no break
                case 'w':
                    $failed_tests_file = fopen($argv[++$i], 'w+t');
                    break;
                case 'a':
                    $failed_tests_file = fopen($argv[++$i], 'a+t');
                    break;
                case 'W':
                    $result_tests_file = fopen($argv[++$i], 'w+t');
                    break;
                case 'c':
                    $conf_passed = $argv[++$i];
                    break;
                case 'd':
                    $ini_overwrites[] = $argv[++$i];
                    break;
                case 'g':
                    $SHOW_ONLY_GROUPS = explode(",", $argv[++$i]);
                    break;
                //case 'h'
                case '--keep-all':
                    foreach ($cfgfiles as $file) {
                        $cfg['keep'][$file] = true;
                    }
                    break;
                //case 'l'
                case 'm':
                    $valgrind = new RuntestsValgrind($environment);
                    break;
                case 'M':
                    $valgrind = new RuntestsValgrind($environment, $argv[++$i]);
                    break;
                case 'n':
                    if (!$pass_option_n) {
                        $pass_options .= ' -n';
                    }
                    $pass_option_n = true;
                    break;
                case 'e':
                    $pass_options .= ' -e';
                    break;
                case '--preload':
                    $preload = true;
                    break;
                case '--file-cache-prime':
                    $file_cache = 'prime';
                    break;
                case '--file-cache-use':
                    $file_cache = 'use';
                    break;
                case '--no-clean':
                    $no_clean = true;
                    break;
                case '--color':
                    $colorize = true;
                    break;
                case '--no-color':
                    $colorize = false;
                    break;
                case 'p':
                    $php = $argv[++$i];
                    putenv("TEST_PHP_EXECUTABLE=$php");
                    $environment['TEST_PHP_EXECUTABLE'] = $php;
                    break;
                case 'P':
                    $php = PHP_BINARY;
                    putenv("TEST_PHP_EXECUTABLE=$php");
                    $environment['TEST_PHP_EXECUTABLE'] = $php;
                    break;
                case 'q':
                    putenv('NO_INTERACTION=1');
                    $environment['NO_INTERACTION'] = 1;
                    break;
                //case 'r'
                case 's':
                    $output_file = $argv[++$i];
                    $just_save_results = true;
                    break;
                case '--set-timeout':
                    $timeout = $argv[++$i] ?? '';
                    if (!preg_match('/^\d+$/', $timeout)) {
                        error("'$timeout' is not a valid number of seconds, try e.g. --set-timeout 60 for 1 minute");
                    }
                    $environment['TEST_TIMEOUT'] = intval($timeout, 10);
                    break;
                case '--context':
                    $context_line_count = $argv[++$i] ?? '';
                    if (!preg_match('/^\d+$/', $context_line_count)) {
                        error("'$context_line_count' is not a valid number of lines of context, try e.g. --context 3 for 3 lines");
                    }
                    $context_line_count = intval($context_line_count, 10);
                    break;
                case '--show-all':
                    foreach ($cfgfiles as $file) {
                        $cfg['show'][$file] = true;
                    }
                    break;
                case '--show-slow':
                    $slow_min_ms = $argv[++$i] ?? '';
                    if (!preg_match('/^\d+$/', $slow_min_ms)) {
                        error("'$slow_min_ms' is not a valid number of milliseconds, try e.g. --show-slow 1000 for 1 second");
                    }
                    $slow_min_ms = intval($slow_min_ms, 10);
                    break;
                case '--temp-source':
                    $temp_source = $argv[++$i];
                    break;
                case '--temp-target':
                    $temp_target = $argv[++$i];
                    break;
                case 'v':
                case '--verbose':
                    $DETAILED = true;
                    break;
                case 'x':
                    $environment['SKIP_SLOW_TESTS'] = 1;
                    break;
                case '--offline':
                    $environment['SKIP_ONLINE_TESTS'] = 1;
                    break;
                case '--shuffle':
                    $shuffle = true;
                    break;
                case '--asan':
                case '--msan':
                    $environment['USE_ZEND_ALLOC'] = 0;
                    $environment['USE_TRACKED_ALLOC'] = 1;
                    $environment['SKIP_ASAN'] = 1;
                    $environment['SKIP_PERF_SENSITIVE'] = 1;
                    if ($switch === '--msan') {
                        $environment['SKIP_MSAN'] = 1;
                    }

                    $lsanSuppressions = __DIR__ . '/.github/lsan-suppressions.txt';
                    if (file_exists($lsanSuppressions)) {
                        $environment['LSAN_OPTIONS'] = 'suppressions=' . $lsanSuppressions
                            . ':print_suppressions=0';
                    }
                    break;
                //case 'w'
                case '-':
                    // repeat check with full switch
                    $switch = $argv[$i];
                    if ($switch != '-') {
                        $repeat = true;
                    }
                    break;
                case '--version':
                    echo '$Id$' . "\n";
                    exit(1);

                default:
                    echo "Illegal switch '$switch' specified!\n";
                    // no break
                case 'h':
                case '-help':
                case '--help':
                    show_usage();
                    exit(1);
            }
        }

        if (!$is_switch) {
            $selected_tests = true;
            $testfile = realpath($argv[$i]);

            if (!$testfile && strpos($argv[$i], '*') !== false && function_exists('glob')) {
                if (substr($argv[$i], -5) == '.phpt') {
                    $pattern_match = glob($argv[$i]);
                } else {
                    if (preg_match("/\*$/", $argv[$i])) {
                        $pattern_match = glob($argv[$i] . '.phpt');
                    } else {
                        die('Cannot find test file "' . $argv[$i] . '".' . PHP_EOL);
                    }
                }

                if (is_array($pattern_match)) {
                    $test_files = array_merge($test_files, $pattern_match);
                }
            } else {
                if (is_dir($testfile)) {
                    find_files($testfile);
                } else {
                    if (substr($testfile, -5) == '.phpt') {
                        $test_files[] = $testfile;
                    } else {
                        die('Cannot find test file "' . $argv[$i] . '".' . PHP_EOL);
                    }
                }
            }
        }
    }

    if ($selected_tests && count($test_files) === 0) {
        echo "No tests found.\n";
        return;
    }

    // Default to PHP_BINARY as executable
    if (!isset($environment['TEST_PHP_EXECUTABLE'])) {
        $php = PHP_BINARY;
        putenv("TEST_PHP_EXECUTABLE=$php");
        $environment['TEST_PHP_EXECUTABLE'] = $php;
    }

    if (strlen($conf_passed)) {
        if (IS_WINDOWS) {
            $pass_options .= " -c " . escapeshellarg($conf_passed);
        } else {
            $pass_options .= " -c '" . realpath($conf_passed) . "'";
        }
    }

    $test_files = array_unique($test_files);
    $test_files = array_merge($test_files, $redir_tests);

    // Run selected tests.
    $test_cnt = count($test_files);

    verify_config();
    write_information();

    if ($test_cnt) {
        putenv('NO_INTERACTION=1');
        usort($test_files, "test_sort");
        $start_time = time();

        echo "Running selected tests.\n";

        $test_idx = 0;
        run_all_tests($test_files, $environment);
        $end_time = time();

        if ($failed_tests_file) {
            fclose($failed_tests_file);
        }

        if ($result_tests_file) {
            fclose($result_tests_file);
        }

        if (0 == count($test_results)) {
            echo "No tests were run.\n";
            return;
        }

        compute_summary();
        echo "=====================================================================";
        echo get_summary(false);

        if ($output_file != '' && $just_save_results) {
            save_or_mail_results();
        }
    } else {
        // Compile a list of all test files (*.phpt).
        $test_files = [];
        $exts_tested = count($exts_to_test);
        $exts_skipped = 0;
        $ignored_by_ext = 0;
        sort($exts_to_test);
        $test_dirs = [];
        $optionals = ['Zend', 'tests', 'ext', 'sapi'];

        foreach ($optionals as $dir) {
            if (is_dir($dir)) {
                $test_dirs[] = $dir;
            }
        }

        // Convert extension names to lowercase
        foreach ($exts_to_test as $key => $val) {
            $exts_to_test[$key] = strtolower($val);
        }

        foreach ($test_dirs as $dir) {
            find_files(TEST_PHP_SRCDIR . "/{$dir}", $dir == 'ext');
        }

        foreach ($user_tests as $dir) {
            find_files($dir, $dir == 'ext');
        }

        $test_files = array_unique($test_files);
        usort($test_files, "test_sort");

        $start_time = time();
        show_start($start_time);

        $test_cnt = count($test_files);
        $test_idx = 0;
        run_all_tests($test_files, $environment);
        $end_time = time();

        if ($failed_tests_file) {
            fclose($failed_tests_file);
        }

        if ($result_tests_file) {
            fclose($result_tests_file);
        }

        // Summarize results

        if (0 == count($test_results)) {
            echo "No tests were run.\n";
            return;
        }

        compute_summary();

        show_end($end_time);
        show_summary();

        save_or_mail_results();
    }

    junit_save_xml();
    if (getenv('REPORT_EXIT_STATUS') !== '0' && getenv('REPORT_EXIT_STATUS') !== 'no' &&
            ($sum_results['FAILED'] || $sum_results['BORKED'] || $sum_results['LEAKED'])) {
        exit(1);
    }
}

if (!function_exists("hrtime")) {
    /**
     * @return array|float|int
     */
    function hrtime(bool $as_num = false)
    {
        $t = microtime(true);

        if ($as_num) {
            return $t * 1000000000;
        }

        $s = floor($t);
        return [0 => $s, 1 => ($t - $s) * 1000000000];
    }
}

function verify_config(): void
{
    global $php;

    if (empty($php) || !file_exists($php)) {
        error('environment variable TEST_PHP_EXECUTABLE must be set to specify PHP executable!');
    }

    if (!is_executable($php)) {
        error("invalid PHP executable specified by TEST_PHP_EXECUTABLE  = $php");
    }
}

function write_information(): void
{
    global $php, $php_cgi, $phpdbg, $php_info, $user_tests, $ini_overwrites, $pass_options, $exts_to_test, $valgrind, $no_file_cache;

    // Get info from php
    $info_file = __DIR__ . '/run-test-info.php';
    @unlink($info_file);
    $php_info = '<?php echo "
PHP_SAPI    : " , PHP_SAPI , "
PHP_VERSION : " , phpversion() , "
ZEND_VERSION: " , zend_version() , "
PHP_OS      : " , PHP_OS , " - " , php_uname() , "
INI actual  : " , realpath(get_cfg_var("cfg_file_path")) , "
More .INIs  : " , (function_exists(\'php_ini_scanned_files\') ? str_replace("\n","", php_ini_scanned_files()) : "** not determined **"); ?>';
    save_text($info_file, $php_info);
    $info_params = [];
    settings2array($ini_overwrites, $info_params);
    $info_params = settings2params($info_params);
    $php_info = `$php $pass_options $info_params $no_file_cache "$info_file"`;
    define('TESTED_PHP_VERSION', `$php -n -r "echo PHP_VERSION;"`);

    if ($php_cgi && $php != $php_cgi) {
        $php_info_cgi = `$php_cgi $pass_options $info_params $no_file_cache -q "$info_file"`;
        $php_info_sep = "\n---------------------------------------------------------------------";
        $php_cgi_info = "$php_info_sep\nPHP         : $php_cgi $php_info_cgi$php_info_sep";
    } else {
        $php_cgi_info = '';
    }

    if ($phpdbg) {
        $phpdbg_info = `$phpdbg $pass_options $info_params $no_file_cache -qrr "$info_file"`;
        $php_info_sep = "\n---------------------------------------------------------------------";
        $phpdbg_info = "$php_info_sep\nPHP         : $phpdbg $phpdbg_info$php_info_sep";
    } else {
        $phpdbg_info = '';
    }

    if (function_exists('opcache_invalidate')) {
        opcache_invalidate($info_file, true);
    }
    @unlink($info_file);

    // load list of enabled extensions
    save_text($info_file,
        '<?php echo str_replace("Zend OPcache", "opcache", implode(",", get_loaded_extensions())); ?>');
    $exts_to_test = explode(',', `$php $pass_options $info_params $no_file_cache "$info_file"`);
    // check for extensions that need special handling and regenerate
    $info_params_ex = [
        'session' => ['session.auto_start=0'],
        'tidy' => ['tidy.clean_output=0'],
        'zlib' => ['zlib.output_compression=Off'],
        'xdebug' => ['xdebug.mode=off'],
        'mbstring' => ['mbstring.func_overload=0'],
    ];

    foreach ($info_params_ex as $ext => $ini_overwrites_ex) {
        if (in_array($ext, $exts_to_test)) {
            $ini_overwrites = array_merge($ini_overwrites, $ini_overwrites_ex);
        }
    }

    if (function_exists('opcache_invalidate')) {
        opcache_invalidate($info_file, true);
    }
    @unlink($info_file);

    // Write test context information.
    echo "
=====================================================================
PHP         : $php $php_info $php_cgi_info $phpdbg_info
CWD         : " . TEST_PHP_SRCDIR . "
Extra dirs  : ";
    foreach ($user_tests as $test_dir) {
        echo "{$test_dir}\n			  ";
    }
    echo "
VALGRIND    : " . ($valgrind ? $valgrind->getHeader() : 'Not used') . "
=====================================================================
";
}

function save_or_mail_results(): void
{
    global $sum_results, $just_save_results, $failed_test_summary,
           $PHP_FAILED_TESTS, $php, $output_file;

    /* We got failed Tests, offer the user to send an e-mail to QA team, unless NO_INTERACTION is set */
    if (!getenv('NO_INTERACTION') && !TRAVIS_CI) {
        $fp = fopen("php://stdin", "r+");
        if ($sum_results['FAILED'] || $sum_results['BORKED'] || $sum_results['WARNED'] || $sum_results['LEAKED']) {
            echo "\nYou may have found a problem in PHP.";
        }
        echo "\nThis report can be automatically sent to the PHP QA team at\n";
        echo QA_REPORTS_PAGE . " and http://news.php.net/php.qa.reports\n";
        echo "This gives us a better understanding of PHP's behavior.\n";
        echo "If you don't want to send the report immediately you can choose\n";
        echo "option \"s\" to save it.	You can then email it to " . PHP_QA_EMAIL . " later.\n";
        echo "Do you want to send this report now? [Yns]: ";
        flush();

        $user_input = fgets($fp, 10);
        $just_save_results = (!empty($user_input) && strtolower($user_input[0]) === 's');
    }

    if ($just_save_results || !getenv('NO_INTERACTION') || TRAVIS_CI) {
        if ($just_save_results || TRAVIS_CI || strlen(trim($user_input)) == 0 || strtolower($user_input[0]) == 'y') {
            /*
             * Collect information about the host system for our report
             * Fetch phpinfo() output so that we can see the PHP environment
             * Make an archive of all the failed tests
             * Send an email
             */
            if ($just_save_results) {
                $user_input = 's';
            }

            /* Ask the user to provide an email address, so that QA team can contact the user */
            if (TRAVIS_CI) {
                $user_email = 'travis at php dot net';
            } elseif (!strncasecmp($user_input, 'y', 1) || strlen(trim($user_input)) == 0) {
                echo "\nPlease enter your email address.\n(Your address will be mangled so that it will not go out on any\nmailinglist in plain text): ";
                flush();
                $user_email = trim(fgets($fp, 1024));
                $user_email = str_replace("@", " at ", str_replace(".", " dot ", $user_email));
            }

            $failed_tests_data = '';
            $sep = "\n" . str_repeat('=', 80) . "\n";
            $failed_tests_data .= $failed_test_summary . "\n";
            $failed_tests_data .= get_summary(true) . "\n";

            if ($sum_results['FAILED']) {
                foreach ($PHP_FAILED_TESTS['FAILED'] as $test_info) {
                    $failed_tests_data .= $sep . $test_info['name'] . $test_info['info'];
                    $failed_tests_data .= $sep . file_get_contents(realpath($test_info['output']));
                    $failed_tests_data .= $sep . file_get_contents(realpath($test_info['diff']));
                    $failed_tests_data .= $sep . "\n\n";
                }
                $status = "failed";
            } else {
                $status = "success";
            }

            $failed_tests_data .= "\n" . $sep . 'BUILD ENVIRONMENT' . $sep;
            $failed_tests_data .= "OS:\n" . PHP_OS . " - " . php_uname() . "\n\n";
            $ldd = $autoconf = $sys_libtool = $libtool = $compiler = 'N/A';

            if (!IS_WINDOWS) {
                /* If PHP_AUTOCONF is set, use it; otherwise, use 'autoconf'. */
                if (getenv('PHP_AUTOCONF')) {
                    $autoconf = shell_exec(getenv('PHP_AUTOCONF') . ' --version');
                } else {
                    $autoconf = shell_exec('autoconf --version');
                }

                /* Always use the generated libtool - Mac OSX uses 'glibtool' */
                $libtool = shell_exec(INIT_DIR . '/libtool --version');

                /* Use shtool to find out if there is glibtool present (MacOSX) */
                $sys_libtool_path = shell_exec(__DIR__ . '/build/shtool path glibtool libtool');

                if ($sys_libtool_path) {
                    $sys_libtool = shell_exec(str_replace("\n", "", $sys_libtool_path) . ' --version');
                }

                /* Try the most common flags for 'version' */
                $flags = ['-v', '-V', '--version'];
                $cc_status = 0;

                foreach ($flags as $flag) {
                    system(getenv('CC') . " $flag >/dev/null 2>&1", $cc_status);
                    if ($cc_status == 0) {
                        $compiler = shell_exec(getenv('CC') . " $flag 2>&1");
                        break;
                    }
                }

                $ldd = shell_exec("ldd $php 2>/dev/null");
            }

            $failed_tests_data .= "Autoconf:\n$autoconf\n";
            $failed_tests_data .= "Bundled Libtool:\n$libtool\n";
            $failed_tests_data .= "System Libtool:\n$sys_libtool\n";
            $failed_tests_data .= "Compiler:\n$compiler\n";
            $failed_tests_data .= "Bison:\n" . shell_exec('bison --version 2>/dev/null') . "\n";
            $failed_tests_data .= "Libraries:\n$ldd\n";
            $failed_tests_data .= "\n";

            if (isset($user_email)) {
                $failed_tests_data .= "User's E-mail: " . $user_email . "\n\n";
            }

            $failed_tests_data .= $sep . "PHPINFO" . $sep;
            $failed_tests_data .= shell_exec($php . ' -ddisplay_errors=stderr -dhtml_errors=0 -i 2> /dev/null');

            if (($just_save_results || !mail_qa_team($failed_tests_data, $status)) && !TRAVIS_CI) {
                file_put_contents($output_file, $failed_tests_data);

                if (!$just_save_results) {
                    echo "\nThe test script was unable to automatically send the report to PHP's QA Team\n";
                }

                echo "Please send " . $output_file . " to " . PHP_QA_EMAIL . " manually, thank you.\n";
            } elseif (!getenv('NO_INTERACTION') && !TRAVIS_CI) {
                fwrite($fp, "\nThank you for helping to make PHP better.\n");
                fclose($fp);
            }
        }
    }
}

function find_files(string $dir, bool $is_ext_dir = false, bool $ignore = false): void
{
    global $test_files, $exts_to_test, $ignored_by_ext, $exts_skipped;

    $o = opendir($dir) or error("cannot open directory: $dir");

    while (($name = readdir($o)) !== false) {
        if (is_dir("{$dir}/{$name}") && !in_array($name, ['.', '..', '.svn'])) {
            $skip_ext = ($is_ext_dir && !in_array(strtolower($name), $exts_to_test));
            if ($skip_ext) {
                $exts_skipped++;
            }
            find_files("{$dir}/{$name}", false, $ignore || $skip_ext);
        }

        // Cleanup any left-over tmp files from last run.
        if (substr($name, -4) == '.tmp') {
            @unlink("$dir/$name");
            continue;
        }

        // Otherwise we're only interested in *.phpt files.
        if (substr($name, -5) == '.phpt') {
            if ($ignore) {
                $ignored_by_ext++;
            } else {
                $testfile = realpath("{$dir}/{$name}");
                $test_files[] = $testfile;
            }
        }
    }

    closedir($o);
}

/**
 * @param array|string $name
 */
function test_name($name): string
{
    if (is_array($name)) {
        return $name[0] . ':' . $name[1];
    } else {
        return $name;
    }
}
/**
 * @param array|string $a
 * @param array|string $b
 */
function test_sort($a, $b): int
{
    $a = test_name($a);
    $b = test_name($b);

    $ta = strpos($a, TEST_PHP_SRCDIR . "/tests") === 0 ? 1 + (strpos($a,
            TEST_PHP_SRCDIR . "/tests/run-test") === 0 ? 1 : 0) : 0;
    $tb = strpos($b, TEST_PHP_SRCDIR . "/tests") === 0 ? 1 + (strpos($b,
            TEST_PHP_SRCDIR . "/tests/run-test") === 0 ? 1 : 0) : 0;

    if ($ta == $tb) {
        return strcmp($a, $b);
    } else {
        return $tb - $ta;
    }
}

//
// Send Email to QA Team
//

function mail_qa_team(string $data, bool $status = false): bool
{
    $url_bits = parse_url(QA_SUBMISSION_PAGE);

    if ($proxy = getenv('http_proxy')) {
        $proxy = parse_url($proxy);
        $path = $url_bits['host'] . $url_bits['path'];
        $host = $proxy['host'];
        if (empty($proxy['port'])) {
            $proxy['port'] = 80;
        }
        $port = $proxy['port'];
    } else {
        $path = $url_bits['path'];
        $host = $url_bits['host'];
        $port = empty($url_bits['port']) ? 80 : $port = $url_bits['port'];
    }

    $data = "php_test_data=" . urlencode(base64_encode(str_replace("\00", '[0x0]', $data)));
    $data_length = strlen($data);

    $fs = fsockopen($host, $port, $errno, $errstr, 10);

    if (!$fs) {
        return false;
    }

    $php_version = urlencode(TESTED_PHP_VERSION);

    echo "\nPosting to " . QA_SUBMISSION_PAGE . "\n";
    fwrite($fs, "POST " . $path . "?status=$status&version=$php_version HTTP/1.1\r\n");
    fwrite($fs, "Host: " . $host . "\r\n");
    fwrite($fs, "User-Agent: QA Browser 0.1\r\n");
    fwrite($fs, "Content-Type: application/x-www-form-urlencoded\r\n");
    fwrite($fs, "Content-Length: " . $data_length . "\r\n\r\n");
    fwrite($fs, $data);
    fwrite($fs, "\r\n\r\n");
    fclose($fs);

    return true;
}

//
//  Write the given text to a temporary file, and return the filename.
//

function save_text(string $filename, string $text, ?string $filename_copy = null): void
{
    global $DETAILED;

    if ($filename_copy && $filename_copy != $filename) {
        if (file_put_contents($filename_copy, $text) === false) {
            error("Cannot open file '" . $filename_copy . "' (save_text)");
        }
    }

    if (file_put_contents($filename, $text) === false) {
        error("Cannot open file '" . $filename . "' (save_text)");
    }

    if (1 < $DETAILED) {
        echo "
FILE $filename {{{
$text
}}}
";
    }
}

//
//  Write an error in a format recognizable to Emacs or MSVC.
//

function error_report(string $testname, string $logname, string $tested): void
{
    $testname = realpath($testname);
    $logname = realpath($logname);

    switch (strtoupper(getenv('TEST_PHP_ERROR_STYLE'))) {
        case 'MSVC':
            echo $testname . "(1) : $tested\n";
            echo $logname . "(1) :  $tested\n";
            break;
        case 'EMACS':
            echo $testname . ":1: $tested\n";
            echo $logname . ":1:  $tested\n";
            break;
    }
}

/**
 * @return false|string
 */
function system_with_timeout(
    string $commandline,
    ?array $env = null,
    ?string $stdin = null,
    bool $captureStdIn = true,
    bool $captureStdOut = true,
    bool $captureStdErr = true
) {
    global $valgrind;

    $data = '';

    $bin_env = [];
    foreach ((array) $env as $key => $value) {
        $bin_env[$key] = $value;
    }

    $descriptorspec = [];
    if ($captureStdIn) {
        $descriptorspec[0] = ['pipe', 'r'];
    }
    if ($captureStdOut) {
        $descriptorspec[1] = ['pipe', 'w'];
    }
    if ($captureStdErr) {
        $descriptorspec[2] = ['pipe', 'w'];
    }
    $proc = proc_open($commandline, $descriptorspec, $pipes, TEST_PHP_SRCDIR, $bin_env, ['suppress_errors' => true]);

    if (!$proc) {
        return false;
    }

    if ($captureStdIn) {
        if (!is_null($stdin)) {
            fwrite($pipes[0], $stdin);
        }
        fclose($pipes[0]);
        unset($pipes[0]);
    }

    $timeout = $valgrind ? 300 : ($env['TEST_TIMEOUT'] ?? 60);

    while (true) {
        /* hide errors from interrupted syscalls */
        $r = $pipes;
        $w = null;
        $e = null;

        $n = @stream_select($r, $w, $e, $timeout);

        if ($n === false) {
            break;
        } elseif ($n === 0) {
            /* timed out */
            $data .= "\n ** ERROR: process timed out **\n";
            proc_terminate($proc, 9);
            return $data;
        } elseif ($n > 0) {
            if ($captureStdOut) {
                $line = fread($pipes[1], 8192);
            } elseif ($captureStdErr) {
                $line = fread($pipes[2], 8192);
            } else {
                $line = '';
            }
            if (strlen($line) == 0) {
                /* EOF */
                break;
            }
            $data .= $line;
        }
    }

    $stat = proc_get_status($proc);

    if ($stat['signaled']) {
        $data .= "\nTermsig=" . $stat['stopsig'] . "\n";
    }
    if ($stat["exitcode"] > 128 && $stat["exitcode"] < 160) {
        $data .= "\nTermsig=" . ($stat["exitcode"] - 128) . "\n";
    } else if (defined('PHP_WINDOWS_VERSION_MAJOR') && (($stat["exitcode"] >> 28) & 0b1111) === 0b1100) {
        // https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-erref/87fba13e-bf06-450e-83b1-9241dc81e781
        $data .= "\nTermsig=" . $stat["exitcode"] . "\n";
    }

    proc_close($proc);
    return $data;
}

/**
 * @param string|array|null $redir_tested
 */
function run_all_tests(array $test_files, array $env, $redir_tested = null): void
{
    global $test_results, $failed_tests_file, $result_tests_file, $php, $test_idx, $file_cache;
    // Parallel testing
    global $PHP_FAILED_TESTS, $workers, $workerID, $workerSock;

    if ($file_cache !== null) {
        /* Automatically skip opcache tests in --file-cache mode,
         * because opcache generally doesn't expect those to run under file cache */
        $test_files = array_filter($test_files, function($test) {
            return !is_string($test) || false === strpos($test, 'ext/opcache');
        });
    }

    /* Ignore -jN if there is only one file to analyze. */
    if ($workers !== null && count($test_files) > 1 && !$workerID) {
        run_all_tests_parallel($test_files, $env, $redir_tested);
        return;
    }

    foreach ($test_files as $name) {
        if (is_array($name)) {
            $index = "# $name[1]: $name[0]";

            if ($redir_tested) {
                $name = $name[0];
            }
        } elseif ($redir_tested) {
            $index = "# $redir_tested: $name";
        } else {
            $index = $name;
        }
        $test_idx++;

        if ($workerID) {
            $PHP_FAILED_TESTS = ['BORKED' => [], 'FAILED' => [], 'WARNED' => [], 'LEAKED' => [], 'XFAILED' => [], 'XLEAKED' => [], 'SLOW' => []];
            ob_start();
        }

        $result = run_test($php, $name, $env);
        if ($workerID) {
            $resultText = ob_get_clean();
        }

        if (!is_array($name) && $result != 'REDIR') {
            if ($workerID) {
                send_message($workerSock, [
                    "type" => "test_result",
                    "name" => $name,
                    "index" => $index,
                    "result" => $result,
                    "text" => $resultText,
                    "PHP_FAILED_TESTS" => $PHP_FAILED_TESTS
                ]);
                continue;
            }

            $test_results[$index] = $result;
            if ($failed_tests_file && ($result == 'XFAILED' || $result == 'XLEAKED' || $result == 'FAILED' || $result == 'WARNED' || $result == 'LEAKED')) {
                fwrite($failed_tests_file, "$index\n");
            }
            if ($result_tests_file) {
                fwrite($result_tests_file, "$result\t$index\n");
            }
        }
    }
}

/** The heart of parallel testing.
 * @param string|array|null $redir_tested
 */
function run_all_tests_parallel(array $test_files, array $env, $redir_tested): void
{
    global $workers, $test_idx, $test_cnt, $test_results, $failed_tests_file, $result_tests_file, $PHP_FAILED_TESTS, $shuffle, $SHOW_ONLY_GROUPS, $valgrind;

    // The PHP binary running run-tests.php, and run-tests.php itself
    // This PHP executable is *not* necessarily the same as the tested version
    $thisPHP = PHP_BINARY;
    $thisScript = __FILE__;

    $workerProcs = [];
    $workerSocks = [];

    echo "=====================================================================\n";
    echo "========= WELCOME TO THE FUTURE: run-tests PARALLEL EDITION =========\n";
    echo "=====================================================================\n";

    // Each test may specify a list of conflict keys. While a test that conflicts with
    // key K is running, no other test that conflicts with K may run. Conflict keys are
    // specified either in the --CONFLICTS-- section, or CONFLICTS file inside a directory.
    $dirConflictsWith = [];
    $fileConflictsWith = [];
    $sequentialTests = [];
    foreach ($test_files as $i => $file) {
        $contents = file_get_contents($file);
        if (preg_match('/^--CONFLICTS--(.+?)^--/ms', $contents, $matches)) {
            $conflicts = parse_conflicts($matches[1]);
        } else {
            // Cache per-directory conflicts in a separate map, so we compute these only once.
            $dir = dirname($file);
            if (!isset($dirConflictsWith[$dir])) {
                $dirConflicts = [];
                if (file_exists($dir . '/CONFLICTS')) {
                    $contents = file_get_contents($dir . '/CONFLICTS');
                    $dirConflicts = parse_conflicts($contents);
                }
                $dirConflictsWith[$dir] = $dirConflicts;
            }
            $conflicts = $dirConflictsWith[$dir];
        }

        // For tests conflicting with "all", no other tests may run in parallel. We'll run these
        // tests separately at the end, when only one worker is left.
        if (in_array('all', $conflicts, true)) {
            $sequentialTests[] = $file;
            unset($test_files[$i]);
        }

        $fileConflictsWith[$file] = $conflicts;
    }

    // Some tests assume that they are executed in a certain order. We will be popping from
    // $test_files, so reverse its order here. This makes sure that order is preserved at least
    // for tests with a common conflict key.
    $test_files = array_reverse($test_files);

    // To discover parallelization issues it is useful to randomize the test order.
    if ($shuffle) {
        shuffle($test_files);
    }

    // Don't start more workers than test files.
    $workers = max(1, min($workers, count($test_files)));

    echo "Spawning $workers workers... ";

    // We use sockets rather than STDIN/STDOUT for comms because on Windows,
    // those can't be non-blocking for some reason.
    $listenSock = stream_socket_server("tcp://127.0.0.1:0") or error("Couldn't create socket on localhost.");
    $sockName = stream_socket_get_name($listenSock, false);
    // PHP is terrible and returns IPv6 addresses not enclosed by []
    $portPos = strrpos($sockName, ":");
    $sockHost = substr($sockName, 0, $portPos);
    if (false !== strpos($sockHost, ":")) {
        $sockHost = "[$sockHost]";
    }
    $sockPort = substr($sockName, $portPos + 1);
    $sockUri = "tcp://$sockHost:$sockPort";
    $totalFileCount = count($test_files);

    $startTime = microtime(true);
    for ($i = 1; $i <= $workers; $i++) {
        $proc = proc_open(
            $thisPHP . ' ' . escapeshellarg($thisScript),
            [], // Inherit our stdin, stdout and stderr
            $pipes,
            null,
            $_ENV + [
                "TEST_PHP_WORKER" => $i,
                "TEST_PHP_URI" => $sockUri,
            ],
            [
                "suppress_errors" => true,
                'create_new_console' => true,
            ]
        );
        if ($proc === false) {
            kill_children($workerProcs);
            error("Failed to spawn worker $i");
        }
        $workerProcs[$i] = $proc;
    }

    for ($i = 1; $i <= $workers; $i++) {
        $workerSock = stream_socket_accept($listenSock, 5);
        if ($workerSock === false) {
            kill_children($workerProcs);
            error("Failed to accept connection from worker.");
        }

        $greeting = base64_encode(serialize([
            "type" => "hello",
            "GLOBALS" => $GLOBALS,
            "constants" => [
                "INIT_DIR" => INIT_DIR,
                "TEST_PHP_SRCDIR" => TEST_PHP_SRCDIR,
                "PHP_QA_EMAIL" => PHP_QA_EMAIL,
                "QA_SUBMISSION_PAGE" => QA_SUBMISSION_PAGE,
                "QA_REPORTS_PAGE" => QA_REPORTS_PAGE,
                "TRAVIS_CI" => TRAVIS_CI
            ]
        ])) . "\n";

        stream_set_timeout($workerSock, 5);
        if (fwrite($workerSock, $greeting) === false) {
            kill_children($workerProcs);
            error("Failed to send greeting to worker.");
        }

        $rawReply = fgets($workerSock);
        if ($rawReply === false) {
            kill_children($workerProcs);
            error("Failed to read greeting reply from worker.");
        }

        $reply = unserialize(base64_decode($rawReply));
        if (!$reply || $reply["type"] !== "hello_reply") {
            kill_children($workerProcs);
            error("Greeting reply from worker unexpected or could not be decoded: '$rawReply'");
        }

        stream_set_timeout($workerSock, 0);
        stream_set_blocking($workerSock, false);

        $workerID = $reply["workerID"];
        $workerSocks[$workerID] = $workerSock;
    }
    printf("Done in %.2fs\n", microtime(true) - $startTime);
    echo "=====================================================================\n";
    echo "\n";

    $rawMessageBuffers = [];
    $testsInProgress = 0;

    // Map from conflict key to worker ID.
    $activeConflicts = [];
    // Tests waiting due to conflicts. Map from conflict key to array.
    $waitingTests = [];

escape:
    while ($test_files || $sequentialTests || $testsInProgress > 0) {
        $toRead = array_values($workerSocks);
        $toWrite = null;
        $toExcept = null;
        if (stream_select($toRead, $toWrite, $toExcept, 10)) {
            foreach ($toRead as $workerSock) {
                $i = array_search($workerSock, $workerSocks);
                if ($i === false) {
                    kill_children($workerProcs);
                    error("Could not find worker stdout in array of worker stdouts, THIS SHOULD NOT HAPPEN.");
                }
                while (false !== ($rawMessage = fgets($workerSock))) {
                    // work around fgets truncating things
                    if (($rawMessageBuffers[$i] ?? '') !== '') {
                        $rawMessage = $rawMessageBuffers[$i] . $rawMessage;
                        $rawMessageBuffers[$i] = '';
                    }
                    if (substr($rawMessage, -1) !== "\n") {
                        $rawMessageBuffers[$i] = $rawMessage;
                        continue;
                    }

                    $message = unserialize(base64_decode($rawMessage));
                    if (!$message) {
                        kill_children($workerProcs);
                        $stuff = fread($workerSock, 65536);
                        error("Could not decode message from worker $i: '$rawMessage$stuff'");
                    }

                    switch ($message["type"]) {
                        case "tests_finished":
                            $testsInProgress--;
                            foreach ($activeConflicts as $key => $workerId) {
                                if ($workerId === $i) {
                                    unset($activeConflicts[$key]);
                                    if (isset($waitingTests[$key])) {
                                        while ($test = array_pop($waitingTests[$key])) {
                                            $test_files[] = $test;
                                        }
                                        unset($waitingTests[$key]);
                                    }
                                }
                            }
                            if (junit_enabled()) {
                                junit_merge_results($message["junit"]);
                            }
                            // no break
                        case "ready":
                            // Schedule sequential tests only once we are down to one worker.
                            if (count($workerProcs) === 1 && $sequentialTests) {
                                $test_files = array_merge($test_files, $sequentialTests);
                                $sequentialTests = [];
                            }
                            // Batch multiple tests to reduce communication overhead.
                            // - When valgrind is used, communication overhead is relatively small,
                            //   so just use a batch size of 1.
                            // - If this is running a small enough number of tests,
                            //   reduce the batch size to give batches to more workers.
                            $files = [];
                            $maxBatchSize = $valgrind ? 1 : ($shuffle ? 4 : 32);
                            $averageFilesPerWorker = max(1, (int) ceil($totalFileCount / count($workerProcs)));
                            $batchSize = min($maxBatchSize, $averageFilesPerWorker);
                            while (count($files) <= $batchSize && $file = array_pop($test_files)) {
                                foreach ($fileConflictsWith[$file] as $conflictKey) {
                                    if (isset($activeConflicts[$conflictKey])) {
                                        $waitingTests[$conflictKey][] = $file;
                                        continue 2;
                                    }
                                }
                                $files[] = $file;
                            }
                            if ($files) {
                                foreach ($files as $file) {
                                    foreach ($fileConflictsWith[$file] as $conflictKey) {
                                        $activeConflicts[$conflictKey] = $i;
                                    }
                                }
                                $testsInProgress++;
                                send_message($workerSocks[$i], [
                                    "type" => "run_tests",
                                    "test_files" => $files,
                                    "env" => $env,
                                    "redir_tested" => $redir_tested
                                ]);
                            } else {
                                proc_terminate($workerProcs[$i]);
                                unset($workerProcs[$i]);
                                unset($workerSocks[$i]);
                                goto escape;
                            }
                            break;
                        case "test_result":
                            list($name, $index, $result, $resultText) = [$message["name"], $message["index"], $message["result"], $message["text"]];
                            foreach ($message["PHP_FAILED_TESTS"] as $category => $tests) {
                                $PHP_FAILED_TESTS[$category] = array_merge($PHP_FAILED_TESTS[$category], $tests);
                            }
                            $test_idx++;

                            if (!$SHOW_ONLY_GROUPS) {
                                clear_show_test();
                            }

                            echo $resultText;

                            if (!$SHOW_ONLY_GROUPS) {
                                show_test($test_idx, count($workerProcs) . "/$workers concurrent test workers running");
                            }

                            if (!is_array($name) && $result != 'REDIR') {
                                $test_results[$index] = $result;

                                if ($failed_tests_file && ($result == 'XFAILED' || $result == 'XLEAKED' || $result == 'FAILED' || $result == 'WARNED' || $result == 'LEAKED')) {
                                    fwrite($failed_tests_file, "$index\n");
                                }
                                if ($result_tests_file) {
                                    fwrite($result_tests_file, "$result\t$index\n");
                                }
                            }
                            break;
                        case "error":
                            kill_children($workerProcs);
                            error("Worker $i reported error: $message[msg]");
                            break;
                        case "php_error":
                            kill_children($workerProcs);
                            $error_consts = [
                                'E_ERROR',
                                'E_WARNING',
                                'E_PARSE',
                                'E_NOTICE',
                                'E_CORE_ERROR',
                                'E_CORE_WARNING',
                                'E_COMPILE_ERROR',
                                'E_COMPILE_WARNING',
                                'E_USER_ERROR',
                                'E_USER_WARNING',
                                'E_USER_NOTICE',
                                'E_STRICT', // TODO Cleanup when removed from Zend Engine.
                                'E_RECOVERABLE_ERROR',
                                'E_DEPRECATED',
                                'E_USER_DEPRECATED'
                            ];
                            $error_consts = array_combine(array_map('constant', $error_consts), $error_consts);
                            error("Worker $i reported unexpected {$error_consts[$message['errno']]}: $message[errstr] in $message[errfile] on line $message[errline]");
                            // no break
                        default:
                            kill_children($workerProcs);
                            error("Unrecognised message type '$message[type]' from worker $i");
                    }
                }
            }
        }
    }

    if (!$SHOW_ONLY_GROUPS) {
        clear_show_test();
    }

    kill_children($workerProcs);

    if ($testsInProgress < 0) {
        error("$testsInProgress test batches “in progress”, which is less than zero. THIS SHOULD NOT HAPPEN.");
    }
}

function send_message($stream, array $message): void
{
    $blocking = stream_get_meta_data($stream)["blocked"];
    stream_set_blocking($stream, true);
    fwrite($stream, base64_encode(serialize($message)) . "\n");
    stream_set_blocking($stream, $blocking);
}

function kill_children(array $children): void
{
    foreach ($children as $child) {
        if ($child) {
            proc_terminate($child);
        }
    }
}

function run_worker(): void
{
    global $workerID, $workerSock;

    $sockUri = getenv("TEST_PHP_URI");

    $workerSock = stream_socket_client($sockUri, $_, $_, 5) or error("Couldn't connect to $sockUri");

    $greeting = fgets($workerSock);
    $greeting = unserialize(base64_decode($greeting)) or die("Could not decode greeting\n");
    if ($greeting["type"] !== "hello") {
        error("Unexpected greeting of type $greeting[type]");
    }

    set_error_handler(function (int $errno, string $errstr, string $errfile, int $errline) use ($workerSock): bool {
        if (error_reporting() & $errno) {
            send_message($workerSock, compact('errno', 'errstr', 'errfile', 'errline') + [
                'type' => 'php_error'
            ]);
        }

        return true;
    });

    foreach ($greeting["GLOBALS"] as $var => $value) {
        if ($var !== "workerID" && $var !== "workerSock" && $var !== "GLOBALS") {
            $GLOBALS[$var] = $value;
        }
    }
    foreach ($greeting["constants"] as $const => $value) {
        define($const, $value);
    }

    send_message($workerSock, [
        "type" => "hello_reply",
        "workerID" => $workerID
    ]);

    send_message($workerSock, [
        "type" => "ready"
    ]);

    while (($command = fgets($workerSock))) {
        $command = unserialize(base64_decode($command));

        switch ($command["type"]) {
            case "run_tests":
                run_all_tests($command["test_files"], $command["env"], $command["redir_tested"]);
                send_message($workerSock, [
                    "type" => "tests_finished",
                    "junit" => junit_enabled() ? $GLOBALS['JUNIT'] : null,
                ]);
                junit_init();
                break;
            default:
                send_message($workerSock, [
                    "type" => "error",
                    "msg" => "Unrecognised message type: $command[type]"
                ]);
                break 2;
        }
    }
}

//
//  Show file or result block
//
function show_file_block(string $file, string $block, ?string $section = null): void
{
    global $cfg;
    global $colorize;

    if ($cfg['show'][$file]) {
        if (is_null($section)) {
            $section = strtoupper($file);
        }
        if ($section === 'DIFF' && $colorize) {
            // '-' is Light Red for removal, '+' is Light Green for addition
            $block = preg_replace('/^[0-9]+\-\s.*$/m', "\e[1;31m\\0\e[0m", $block);
            $block = preg_replace('/^[0-9]+\+\s.*$/m', "\e[1;32m\\0\e[0m", $block);
        }

        echo "\n========" . $section . "========\n";
        echo rtrim($block);
        echo "\n========DONE========\n";
    }
}

//
//  Run an individual test case.
//
/**
 * @param string|array $file
 */
function run_test(string $php, $file, array $env): string
{
    global $log_format, $ini_overwrites, $PHP_FAILED_TESTS;
    global $pass_options, $DETAILED, $IN_REDIRECT, $test_cnt, $test_idx;
    global $valgrind, $temp_source, $temp_target, $cfg, $environment;
    global $no_clean;
    global $SHOW_ONLY_GROUPS;
    global $no_file_cache;
    global $slow_min_ms;
    global $preload, $file_cache;
    // Parallel testing
    global $workerID;
    $temp_filenames = null;
    $org_file = $file;

    if (isset($env['TEST_PHP_CGI_EXECUTABLE'])) {
        $php_cgi = $env['TEST_PHP_CGI_EXECUTABLE'];
    }

    if (isset($env['TEST_PHPDBG_EXECUTABLE'])) {
        $phpdbg = $env['TEST_PHPDBG_EXECUTABLE'];
    }

    if (is_array($file)) {
        $file = $file[0];
    }

    if ($DETAILED) {
        echo "
=================
TEST $file
";
    }

    // Load the sections of the test file.
    $section_text = ['TEST' => ''];

    $fp = fopen($file, "rb") or error("Cannot open test file: $file");

    $bork_info = null;

    if (!feof($fp)) {
        $line = fgets($fp);

        if ($line === false) {
            $bork_info = "cannot read test";
        }
    } else {
        $bork_info = "empty test [$file]";
    }
    if ($bork_info === null && strncmp('--TEST--', $line, 8)) {
        $bork_info = "tests must start with --TEST-- [$file]";
    }

    $section = 'TEST';
    $secfile = false;
    $secdone = false;

    while (!feof($fp)) {
        $line = fgets($fp);

        if ($line === false) {
            break;
        }

        // Match the beginning of a section.
        if (preg_match('/^--([_A-Z]+)--/', $line, $r)) {
            $section = (string) $r[1];

            if (isset($section_text[$section]) && $section_text[$section]) {
                $bork_info = "duplicated $section section";
            }

            // check for unknown sections
            if (!in_array($section, [
                'EXPECT', 'EXPECTF', 'EXPECTREGEX', 'EXPECTREGEX_EXTERNAL', 'EXPECT_EXTERNAL', 'EXPECTF_EXTERNAL', 'EXPECTHEADERS',
                'POST', 'POST_RAW', 'GZIP_POST', 'DEFLATE_POST', 'PUT', 'GET', 'COOKIE', 'ARGS',
                'FILE', 'FILEEOF', 'FILE_EXTERNAL', 'REDIRECTTEST',
                'CAPTURE_STDIO', 'STDIN', 'CGI', 'PHPDBG',
                'INI', 'ENV', 'EXTENSIONS',
                'SKIPIF', 'XFAIL', 'XLEAK', 'CLEAN',
                'CREDITS', 'DESCRIPTION', 'CONFLICTS', 'WHITESPACE_SENSITIVE',
            ])) {
                $bork_info = 'Unknown section "' . $section . '"';
            }

            $section_text[$section] = '';
            $secfile = $section == 'FILE' || $section == 'FILEEOF' || $section == 'FILE_EXTERNAL';
            $secdone = false;
            continue;
        }

        // Add to the section text.
        if (!$secdone) {
            $section_text[$section] .= $line;
        }

        // End of actual test?
        if ($secfile && preg_match('/^===DONE===\s*$/', $line)) {
            $secdone = true;
        }
    }

    // the redirect section allows a set of tests to be reused outside of
    // a given test dir
    if ($bork_info === null) {
        if (isset($section_text['REDIRECTTEST'])) {
            if ($IN_REDIRECT) {
                $bork_info = "Can't redirect a test from within a redirected test";
            }
        } else {
            if (!isset($section_text['PHPDBG']) && isset($section_text['FILE']) + isset($section_text['FILEEOF']) + isset($section_text['FILE_EXTERNAL']) != 1) {
                $bork_info = "missing section --FILE--";
            }

            if (isset($section_text['FILEEOF'])) {
                $section_text['FILE'] = preg_replace("/[\r\n]+$/", '', $section_text['FILEEOF']);
                unset($section_text['FILEEOF']);
            }

            foreach (['FILE', 'EXPECT', 'EXPECTF', 'EXPECTREGEX'] as $prefix) {
                $key = $prefix . '_EXTERNAL';

                if (isset($section_text[$key])) {
                    // don't allow tests to retrieve files from anywhere but this subdirectory
                    $section_text[$key] = dirname($file) . '/' . trim(str_replace('..', '', $section_text[$key]));

                    if (file_exists($section_text[$key])) {
                        $section_text[$prefix] = file_get_contents($section_text[$key]);
                        unset($section_text[$key]);
                    } else {
                        $bork_info = "could not load --" . $key . "-- " . dirname($file) . '/' . trim($section_text[$key]);
                    }
                }
            }

            if ((isset($section_text['EXPECT']) + isset($section_text['EXPECTF']) + isset($section_text['EXPECTREGEX'])) != 1) {
                $bork_info = "missing section --EXPECT--, --EXPECTF-- or --EXPECTREGEX--";
            }
        }
    }
    fclose($fp);

    $shortname = str_replace(TEST_PHP_SRCDIR . '/', '', $file);
    $tested_file = $shortname;

    if ($bork_info !== null) {
        show_result("BORK", $bork_info, $tested_file);
        $PHP_FAILED_TESTS['BORKED'][] = [
            'name' => $file,
            'test_name' => '',
            'output' => '',
            'diff' => '',
            'info' => "$bork_info [$file]",
        ];

        junit_mark_test_as('BORK', $shortname, $tested_file, 0, $bork_info);
        return 'BORKED';
    }

    if (isset($section_text['CAPTURE_STDIO'])) {
        $captureStdIn = stripos($section_text['CAPTURE_STDIO'], 'STDIN') !== false;
        $captureStdOut = stripos($section_text['CAPTURE_STDIO'], 'STDOUT') !== false;
        $captureStdErr = stripos($section_text['CAPTURE_STDIO'], 'STDERR') !== false;
    } else {
        $captureStdIn = true;
        $captureStdOut = true;
        $captureStdErr = true;
    }
    if ($captureStdOut && $captureStdErr) {
        $cmdRedirect = ' 2>&1';
    } else {
        $cmdRedirect = '';
    }

    $tested = trim($section_text['TEST']);

    /* For GET/POST/PUT tests, check if cgi sapi is available and if it is, use it. */
    if (array_key_exists('CGI', $section_text) || !empty($section_text['GET']) || !empty($section_text['POST']) || !empty($section_text['GZIP_POST']) || !empty($section_text['DEFLATE_POST']) || !empty($section_text['POST_RAW']) || !empty($section_text['PUT']) || !empty($section_text['COOKIE']) || !empty($section_text['EXPECTHEADERS'])) {
        if (isset($php_cgi)) {
            $php = $php_cgi . ' -C ';
        } elseif (IS_WINDOWS && file_exists(dirname($php) . "/php-cgi.exe")) {
            $php = realpath(dirname($php) . "/php-cgi.exe") . ' -C ';
        } else {
            if (file_exists(dirname($php) . "/../../sapi/cgi/php-cgi")) {
                $php = realpath(dirname($php) . "/../../sapi/cgi/php-cgi") . ' -C ';
            } elseif (file_exists("./sapi/cgi/php-cgi")) {
                $php = realpath("./sapi/cgi/php-cgi") . ' -C ';
            } elseif (file_exists(dirname($php) . "/php-cgi")) {
                $php = realpath(dirname($php) . "/php-cgi") . ' -C ';
            } else {
                show_result('SKIP', $tested, $tested_file, "reason: CGI not available");

                junit_init_suite(junit_get_suitename_for($shortname));
                junit_mark_test_as('SKIP', $shortname, $tested, 0, 'CGI not available');
                return 'SKIPPED';
            }
        }
        $uses_cgi = true;
    }

    /* For phpdbg tests, check if phpdbg sapi is available and if it is, use it. */
    $extra_options = '';
    if (array_key_exists('PHPDBG', $section_text)) {
        if (!isset($section_text['STDIN'])) {
            $section_text['STDIN'] = $section_text['PHPDBG'] . "\n";
        }

        if (isset($phpdbg)) {
            $php = $phpdbg . ' -qIb';

            // Additional phpdbg command line options for sections that need to
            // be run straight away. For example, EXTENSIONS, SKIPIF, CLEAN.
            $extra_options = '-rr';
        } else {
            show_result('SKIP', $tested, $tested_file, "reason: phpdbg not available");

            junit_init_suite(junit_get_suitename_for($shortname));
            junit_mark_test_as('SKIP', $shortname, $tested, 0, 'phpdbg not available');
            return 'SKIPPED';
        }
    }

    if (!$SHOW_ONLY_GROUPS && !$workerID) {
        show_test($test_idx, $shortname);
    }

    if (is_array($IN_REDIRECT)) {
        $temp_dir = $test_dir = $IN_REDIRECT['dir'];
    } else {
        $temp_dir = $test_dir = realpath(dirname($file));
    }

    if ($temp_source && $temp_target) {
        $temp_dir = str_replace($temp_source, $temp_target, $temp_dir);
    }

    $main_file_name = basename($file, 'phpt');

    $diff_filename = $temp_dir . DIRECTORY_SEPARATOR . $main_file_name . 'diff';
    $log_filename = $temp_dir . DIRECTORY_SEPARATOR . $main_file_name . 'log';
    $exp_filename = $temp_dir . DIRECTORY_SEPARATOR . $main_file_name . 'exp';
    $output_filename = $temp_dir . DIRECTORY_SEPARATOR . $main_file_name . 'out';
    $memcheck_filename = $temp_dir . DIRECTORY_SEPARATOR . $main_file_name . 'mem';
    $sh_filename = $temp_dir . DIRECTORY_SEPARATOR . $main_file_name . 'sh';
    $temp_file = $temp_dir . DIRECTORY_SEPARATOR . $main_file_name . 'php';
    $test_file = $test_dir . DIRECTORY_SEPARATOR . $main_file_name . 'php';
    $temp_skipif = $temp_dir . DIRECTORY_SEPARATOR . $main_file_name . 'skip.php';
    $test_skipif = $test_dir . DIRECTORY_SEPARATOR . $main_file_name . 'skip.php';
    $temp_clean = $temp_dir . DIRECTORY_SEPARATOR . $main_file_name . 'clean.php';
    $test_clean = $test_dir . DIRECTORY_SEPARATOR . $main_file_name . 'clean.php';
    $preload_filename = $temp_dir . DIRECTORY_SEPARATOR . $main_file_name . 'preload.php';
    $tmp_post = $temp_dir . DIRECTORY_SEPARATOR . $main_file_name . 'post';
    $tmp_relative_file = str_replace(__DIR__ . DIRECTORY_SEPARATOR, '', $test_file) . 't';

    if ($temp_source && $temp_target) {
        $temp_skipif .= 's';
        $temp_file .= 's';
        $temp_clean .= 's';
        $copy_file = $temp_dir . DIRECTORY_SEPARATOR . basename(is_array($file) ? $file[1] : $file) . '.phps';

        if (!is_dir(dirname($copy_file))) {
            mkdir(dirname($copy_file), 0777, true) or error("Cannot create output directory - " . dirname($copy_file));
        }

        if (isset($section_text['FILE'])) {
            save_text($copy_file, $section_text['FILE']);
        }

        $temp_filenames = [
            'file' => $copy_file,
            'diff' => $diff_filename,
            'log' => $log_filename,
            'exp' => $exp_filename,
            'out' => $output_filename,
            'mem' => $memcheck_filename,
            'sh' => $sh_filename,
            'php' => $temp_file,
            'skip' => $temp_skipif,
            'clean' => $temp_clean
        ];
    }

    if (is_array($IN_REDIRECT)) {
        $tested = $IN_REDIRECT['prefix'] . ' ' . trim($section_text['TEST']);
        $tested_file = $tmp_relative_file;
        $shortname = str_replace(TEST_PHP_SRCDIR . '/', '', $tested_file);
    }

    // unlink old test results
    @unlink($diff_filename);
    @unlink($log_filename);
    @unlink($exp_filename);
    @unlink($output_filename);
    @unlink($memcheck_filename);
    @unlink($sh_filename);
    @unlink($temp_file);
    @unlink($test_file);
    @unlink($temp_skipif);
    @unlink($test_skipif);
    @unlink($tmp_post);
    @unlink($temp_clean);
    @unlink($test_clean);
    @unlink($preload_filename);

    // Reset environment from any previous test.
    $env['REDIRECT_STATUS'] = '';
    $env['QUERY_STRING'] = '';
    $env['PATH_TRANSLATED'] = '';
    $env['SCRIPT_FILENAME'] = '';
    $env['REQUEST_METHOD'] = '';
    $env['CONTENT_TYPE'] = '';
    $env['CONTENT_LENGTH'] = '';
    $env['TZ'] = '';

    if (!empty($section_text['ENV'])) {
        foreach (explode("\n", trim($section_text['ENV'])) as $e) {
            $e = explode('=', trim($e), 2);

            if (!empty($e[0]) && isset($e[1])) {
                $env[$e[0]] = $e[1];
            }
        }
    }

    // Default ini settings
    $ini_settings = $workerID ? ['opcache.cache_id' => "worker$workerID"] : [];

    // Additional required extensions
    if (array_key_exists('EXTENSIONS', $section_text)) {
        $ext_params = [];
        settings2array($ini_overwrites, $ext_params);
        $ext_params = settings2params($ext_params);
        $ext_dir = `$php $pass_options $extra_options $ext_params $no_file_cache -d display_errors=0 -r "echo ini_get('extension_dir');"`;
        $extensions = preg_split("/[\n\r]+/", trim($section_text['EXTENSIONS']));
        $loaded = explode(",", `$php $pass_options $extra_options $ext_params $no_file_cache -d display_errors=0 -r "echo implode(',', get_loaded_extensions());"`);
        $ext_prefix = IS_WINDOWS ? "php_" : "";
        foreach ($extensions as $req_ext) {
            if (!in_array($req_ext, $loaded)) {
                if ($req_ext == 'opcache') {
                    $ini_settings['zend_extension'][] = $ext_dir . DIRECTORY_SEPARATOR . $ext_prefix . $req_ext . '.' . PHP_SHLIB_SUFFIX;
                } else {
                    $ini_settings['extension'][] = $ext_dir . DIRECTORY_SEPARATOR . $ext_prefix . $req_ext . '.' . PHP_SHLIB_SUFFIX;
                }
            }
        }
    }

    // additional ini overwrites
    //$ini_overwrites[] = 'setting=value';
    settings2array($ini_overwrites, $ini_settings);

    $orig_ini_settings = settings2params($ini_settings);

    if ($file_cache !== null) {
        $ini_settings['opcache.file_cache'] = '/tmp';
        // Make sure warnings still show up on the second run.
        $ini_settings['opcache.record_warnings'] = '1';
        // File cache is currently incompatible with JIT.
        $ini_settings['opcache.jit'] = '0';
        if ($file_cache === 'use') {
            // Disable timestamp validation in order to fetch from file cache,
            // even though all the files are re-created.
            $ini_settings['opcache.validate_timestamps'] = '0';
        }
    }

    // Any special ini settings
    // these may overwrite the test defaults...
    if (array_key_exists('INI', $section_text)) {
        $section_text['INI'] = str_replace('{PWD}', dirname($file), $section_text['INI']);
        $section_text['INI'] = str_replace('{TMP}', sys_get_temp_dir(), $section_text['INI']);
        $replacement = IS_WINDOWS ? '"' . PHP_BINARY . ' -r \"while ($in = fgets(STDIN)) echo $in;\" > $1"' : 'tee $1 >/dev/null';
        $section_text['INI'] = preg_replace('/{MAIL:(\S+)}/', $replacement, $section_text['INI']);
        settings2array(preg_split("/[\n\r]+/", $section_text['INI']), $ini_settings);
    }

    $ini_settings = settings2params($ini_settings);

    $env['TEST_PHP_EXTRA_ARGS'] = $pass_options . ' ' . $ini_settings;

    // Check if test should be skipped.
    $info = '';
    $warn = false;

    if (array_key_exists('SKIPIF', $section_text)) {
        if (trim($section_text['SKIPIF'])) {
            show_file_block('skip', $section_text['SKIPIF']);
            save_text($test_skipif, $section_text['SKIPIF'], $temp_skipif);
            $extra = !IS_WINDOWS ?
                "unset REQUEST_METHOD; unset QUERY_STRING; unset PATH_TRANSLATED; unset SCRIPT_FILENAME; unset REQUEST_METHOD;" : "";

            if ($valgrind) {
                $env['USE_ZEND_ALLOC'] = '0';
                $env['ZEND_DONT_UNLOAD_MODULES'] = 1;
            }

            junit_start_timer($shortname);

            $output = system_with_timeout("$extra $php $pass_options $extra_options -q $orig_ini_settings $no_file_cache -d display_errors=1 -d display_startup_errors=0 \"$test_skipif\"", $env);
            $output = trim($output);

            junit_finish_timer($shortname);

            if (!$cfg['keep']['skip']) {
                @unlink($test_skipif);
            }

            if (!strncasecmp('skip', $output, 4)) {
                if (preg_match('/^skip\s*(.+)/i', $output, $m)) {
                    show_result('SKIP', $tested, $tested_file, "reason: $m[1]", $temp_filenames);
                } else {
                    show_result('SKIP', $tested, $tested_file, '', $temp_filenames);
                }

                if (!$cfg['keep']['skip']) {
                    @unlink($test_skipif);
                }

                $message = !empty($m[1]) ? $m[1] : '';
                junit_mark_test_as('SKIP', $shortname, $tested, null, $message);
                return 'SKIPPED';
            }

            if (!strncasecmp('info', $output, 4) && preg_match('/^info\s*(.+)/i', $output, $m)) {
                $info = " (info: $m[1])";
            } elseif (!strncasecmp('warn', $output, 4) && preg_match('/^warn\s+(.+)/i', $output, $m)) {
                $warn = true; /* only if there is a reason */
                $info = " (warn: $m[1])";
            } elseif (!strncasecmp('xfail', $output, 5)) {
                // Pretend we have an XFAIL section
                $section_text['XFAIL'] = ltrim(substr($output, 5));
            } elseif ($output !== '') {
                show_result("BORK", $output, $tested_file, 'reason: invalid output from SKIPIF', $temp_filenames);
                $PHP_FAILED_TESTS['BORKED'][] = [
                    'name' => $file,
                    'test_name' => '',
                    'output' => '',
                    'diff' => '',
                    'info' => "$output [$file]",
                ];

                junit_mark_test_as('BORK', $shortname, $tested, null, $output);
                return 'BORKED';
            }
        }
    }

    if (!extension_loaded("zlib")
        && (array_key_exists("GZIP_POST", $section_text)
        || array_key_exists("DEFLATE_POST", $section_text))) {
        $message = "ext/zlib required";
        show_result('SKIP', $tested, $tested_file, "reason: $message", $temp_filenames);
        junit_mark_test_as('SKIP', $shortname, $tested, null, $message);
        return 'SKIPPED';
    }

    if (isset($section_text['REDIRECTTEST'])) {
        $test_files = [];

        $IN_REDIRECT = eval($section_text['REDIRECTTEST']);
        $IN_REDIRECT['via'] = "via [$shortname]\n\t";
        $IN_REDIRECT['dir'] = realpath(dirname($file));
        $IN_REDIRECT['prefix'] = trim($section_text['TEST']);

        if (!empty($IN_REDIRECT['TESTS'])) {
            if (is_array($org_file)) {
                $test_files[] = $org_file[1];
            } else {
                $GLOBALS['test_files'] = $test_files;
                find_files($IN_REDIRECT['TESTS']);

                foreach ($GLOBALS['test_files'] as $f) {
                    $test_files[] = [$f, $file];
                }
            }
            $test_cnt += count($test_files) - 1;
            $test_idx--;

            show_redirect_start($IN_REDIRECT['TESTS'], $tested, $tested_file);

            // set up environment
            $redirenv = array_merge($environment, $IN_REDIRECT['ENV']);
            $redirenv['REDIR_TEST_DIR'] = realpath($IN_REDIRECT['TESTS']) . DIRECTORY_SEPARATOR;

            usort($test_files, "test_sort");
            run_all_tests($test_files, $redirenv, $tested);

            show_redirect_ends($IN_REDIRECT['TESTS'], $tested, $tested_file);

            // a redirected test never fails
            $IN_REDIRECT = false;

            junit_mark_test_as('PASS', $shortname, $tested);
            return 'REDIR';
        } else {
            $bork_info = "Redirect info must contain exactly one TEST string to be used as redirect directory.";
            show_result("BORK", $bork_info, '', '', $temp_filenames);
            $PHP_FAILED_TESTS['BORKED'][] = [
                'name' => $file,
                'test_name' => '',
                'output' => '',
                'diff' => '',
                'info' => "$bork_info [$file]",
            ];
        }
    }

    if (is_array($org_file) || isset($section_text['REDIRECTTEST'])) {
        if (is_array($org_file)) {
            $file = $org_file[0];
        }

        $bork_info = "Redirected test did not contain redirection info";
        show_result("BORK", $bork_info, '', '', $temp_filenames);
        $PHP_FAILED_TESTS['BORKED'][] = [
            'name' => $file,
            'test_name' => '',
            'output' => '',
            'diff' => '',
            'info' => "$bork_info [$file]",
        ];

        junit_mark_test_as('BORK', $shortname, $tested, null, $bork_info);

        return 'BORKED';
    }

    // We've satisfied the preconditions - run the test!
    if (isset($section_text['FILE'])) {
        show_file_block('php', $section_text['FILE'], 'TEST');
        save_text($test_file, $section_text['FILE'], $temp_file);
    } else {
        $test_file = $temp_file = "";
    }

    if (array_key_exists('GET', $section_text)) {
        $query_string = trim($section_text['GET']);
    } else {
        $query_string = '';
    }

    $env['REDIRECT_STATUS'] = '1';
    if (empty($env['QUERY_STRING'])) {
        $env['QUERY_STRING'] = $query_string;
    }
    if (empty($env['PATH_TRANSLATED'])) {
        $env['PATH_TRANSLATED'] = $test_file;
    }
    if (empty($env['SCRIPT_FILENAME'])) {
        $env['SCRIPT_FILENAME'] = $test_file;
    }

    if (array_key_exists('COOKIE', $section_text)) {
        $env['HTTP_COOKIE'] = trim($section_text['COOKIE']);
    } else {
        $env['HTTP_COOKIE'] = '';
    }

    $args = isset($section_text['ARGS']) ? ' -- ' . $section_text['ARGS'] : '';

    if ($preload && !empty($test_file)) {
        save_text($preload_filename, "<?php opcache_compile_file('$test_file');");
        $local_pass_options = $pass_options;
        unset($pass_options);
        $pass_options = $local_pass_options;
        $pass_options .= " -d opcache.preload=" . $preload_filename;
    }

    if (array_key_exists('POST_RAW', $section_text) && !empty($section_text['POST_RAW'])) {
        $post = trim($section_text['POST_RAW']);
        $raw_lines = explode("\n", $post);

        $request = '';
        $started = false;

        foreach ($raw_lines as $line) {
            if (empty($env['CONTENT_TYPE']) && preg_match('/^Content-Type:(.*)/i', $line, $res)) {
                $env['CONTENT_TYPE'] = trim(str_replace("\r", '', $res[1]));
                continue;
            }

            if ($started) {
                $request .= "\n";
            }

            $started = true;
            $request .= $line;
        }

        $env['CONTENT_LENGTH'] = strlen($request);
        $env['REQUEST_METHOD'] = 'POST';

        if (empty($request)) {
            junit_mark_test_as('BORK', $shortname, $tested, null, 'empty $request');
            return 'BORKED';
        }

        save_text($tmp_post, $request);
        $cmd = "$php $pass_options $ini_settings -f \"$test_file\"$cmdRedirect < \"$tmp_post\"";
    } elseif (array_key_exists('PUT', $section_text) && !empty($section_text['PUT'])) {
        $post = trim($section_text['PUT']);
        $raw_lines = explode("\n", $post);

        $request = '';
        $started = false;

        foreach ($raw_lines as $line) {
            if (empty($env['CONTENT_TYPE']) && preg_match('/^Content-Type:(.*)/i', $line, $res)) {
                $env['CONTENT_TYPE'] = trim(str_replace("\r", '', $res[1]));
                continue;
            }

            if ($started) {
                $request .= "\n";
            }

            $started = true;
            $request .= $line;
        }

        $env['CONTENT_LENGTH'] = strlen($request);
        $env['REQUEST_METHOD'] = 'PUT';

        if (empty($request)) {
            junit_mark_test_as('BORK', $shortname, $tested, null, 'empty $request');
            return 'BORKED';
        }

        save_text($tmp_post, $request);
        $cmd = "$php $pass_options $ini_settings -f \"$test_file\"$cmdRedirect < \"$tmp_post\"";
    } elseif (array_key_exists('POST', $section_text) && !empty($section_text['POST'])) {
        $post = trim($section_text['POST']);
        $content_length = strlen($post);
        save_text($tmp_post, $post);

        $env['REQUEST_METHOD'] = 'POST';
        if (empty($env['CONTENT_TYPE'])) {
            $env['CONTENT_TYPE'] = 'application/x-www-form-urlencoded';
        }

        if (empty($env['CONTENT_LENGTH'])) {
            $env['CONTENT_LENGTH'] = $content_length;
        }

        $cmd = "$php $pass_options $ini_settings -f \"$test_file\"$cmdRedirect < \"$tmp_post\"";
    } elseif (array_key_exists('GZIP_POST', $section_text) && !empty($section_text['GZIP_POST'])) {
        $post = trim($section_text['GZIP_POST']);
        $post = gzencode($post, 9, FORCE_GZIP);
        $env['HTTP_CONTENT_ENCODING'] = 'gzip';

        save_text($tmp_post, $post);
        $content_length = strlen($post);

        $env['REQUEST_METHOD'] = 'POST';
        $env['CONTENT_TYPE'] = 'application/x-www-form-urlencoded';
        $env['CONTENT_LENGTH'] = $content_length;

        $cmd = "$php $pass_options $ini_settings -f \"$test_file\"$cmdRedirect < \"$tmp_post\"";
    } elseif (array_key_exists('DEFLATE_POST', $section_text) && !empty($section_text['DEFLATE_POST'])) {
        $post = trim($section_text['DEFLATE_POST']);
        $post = gzcompress($post, 9);
        $env['HTTP_CONTENT_ENCODING'] = 'deflate';
        save_text($tmp_post, $post);
        $content_length = strlen($post);

        $env['REQUEST_METHOD'] = 'POST';
        $env['CONTENT_TYPE'] = 'application/x-www-form-urlencoded';
        $env['CONTENT_LENGTH'] = $content_length;

        $cmd = "$php $pass_options $ini_settings -f \"$test_file\"$cmdRedirect < \"$tmp_post\"";
    } else {
        $env['REQUEST_METHOD'] = 'GET';
        $env['CONTENT_TYPE'] = '';
        $env['CONTENT_LENGTH'] = '';

        $cmd = "$php $pass_options $ini_settings -f \"$test_file\" $args$cmdRedirect";
    }

    if ($valgrind) {
        $env['USE_ZEND_ALLOC'] = '0';
        $env['ZEND_DONT_UNLOAD_MODULES'] = 1;

        $cmd = $valgrind->wrapCommand($cmd, $memcheck_filename, strpos($test_file, "pcre") !== false);
    }

    if ($DETAILED) {
        echo "
CONTENT_LENGTH  = " . $env['CONTENT_LENGTH'] . "
CONTENT_TYPE    = " . $env['CONTENT_TYPE'] . "
PATH_TRANSLATED = " . $env['PATH_TRANSLATED'] . "
QUERY_STRING    = " . $env['QUERY_STRING'] . "
REDIRECT_STATUS = " . $env['REDIRECT_STATUS'] . "
REQUEST_METHOD  = " . $env['REQUEST_METHOD'] . "
SCRIPT_FILENAME = " . $env['SCRIPT_FILENAME'] . "
HTTP_COOKIE     = " . $env['HTTP_COOKIE'] . "
COMMAND $cmd
";
    }

    junit_start_timer($shortname);
    $hrtime = hrtime();
    $startTime = $hrtime[0] * 1000000000 + $hrtime[1];

    $out = system_with_timeout($cmd, $env, $section_text['STDIN'] ?? null, $captureStdIn, $captureStdOut, $captureStdErr);

    junit_finish_timer($shortname);
    $hrtime = hrtime();
    $time = $hrtime[0] * 1000000000 + $hrtime[1] - $startTime;
    if ($time >= $slow_min_ms * 1000000) {
        $PHP_FAILED_TESTS['SLOW'][] = [
            'name' => $file,
            'test_name' => (is_array($IN_REDIRECT) ? $IN_REDIRECT['via'] : '') . $tested . " [$tested_file]",
            'output' => '',
            'diff' => '',
            'info' => $time / 1000000000,
        ];
    }

    if (array_key_exists('CLEAN', $section_text) && (!$no_clean || $cfg['keep']['clean'])) {
        if (trim($section_text['CLEAN'])) {
            show_file_block('clean', $section_text['CLEAN']);
            save_text($test_clean, trim($section_text['CLEAN']), $temp_clean);

            if (!$no_clean) {
                $extra = !IS_WINDOWS ?
                    "unset REQUEST_METHOD; unset QUERY_STRING; unset PATH_TRANSLATED; unset SCRIPT_FILENAME; unset REQUEST_METHOD;" : "";
                system_with_timeout("$extra $php $pass_options $extra_options -q $orig_ini_settings $no_file_cache \"$test_clean\"", $env);
            }

            if (!$cfg['keep']['clean']) {
                @unlink($test_clean);
            }
        }
    }

    @unlink($preload_filename);

    $leaked = false;
    $passed = false;

    if ($valgrind) { // leak check
        $leaked = filesize($memcheck_filename) > 0;

        if (!$leaked) {
            @unlink($memcheck_filename);
        }
    }

    // Does the output match what is expected?
    $output = preg_replace("/\r\n/", "\n", trim($out));

    /* when using CGI, strip the headers from the output */
    $headers = [];

    if (!empty($uses_cgi) && preg_match("/^(.*?)\r?\n\r?\n(.*)/s", $out, $match)) {
        $output = trim($match[2]);
        $rh = preg_split("/[\n\r]+/", $match[1]);

        foreach ($rh as $line) {
            if (strpos($line, ':') !== false) {
                $line = explode(':', $line, 2);
                $headers[trim($line[0])] = trim($line[1]);
            }
        }
    }

    $failed_headers = false;

    if (isset($section_text['EXPECTHEADERS'])) {
        $want = [];
        $wanted_headers = [];
        $lines = preg_split("/[\n\r]+/", $section_text['EXPECTHEADERS']);

        foreach ($lines as $line) {
            if (strpos($line, ':') !== false) {
                $line = explode(':', $line, 2);
                $want[trim($line[0])] = trim($line[1]);
                $wanted_headers[] = trim($line[0]) . ': ' . trim($line[1]);
            }
        }

        $output_headers = [];

        foreach ($want as $k => $v) {
            if (isset($headers[$k])) {
                $output_headers[] = $k . ': ' . $headers[$k];
            }

            if (!isset($headers[$k]) || $headers[$k] != $v) {
                $failed_headers = true;
            }
        }

        ksort($wanted_headers);
        $wanted_headers = implode("\n", $wanted_headers);
        ksort($output_headers);
        $output_headers = implode("\n", $output_headers);
    }

    show_file_block('out', $output);

    if ($preload) {
        $output = trim(preg_replace("/\n?Warning: Can't preload [^\n]*\n?/", "", $output));
    }

    if (isset($section_text['EXPECTF']) || isset($section_text['EXPECTREGEX'])) {
        if (isset($section_text['EXPECTF'])) {
            $wanted = trim($section_text['EXPECTF']);
        } else {
            $wanted = trim($section_text['EXPECTREGEX']);
        }

        show_file_block('exp', $wanted);
        $wanted_re = preg_replace('/\r\n/', "\n", $wanted);

        if (isset($section_text['EXPECTF'])) {
            // do preg_quote, but miss out any %r delimited sections
            $temp = "";
            $r = "%r";
            $startOffset = 0;
            $length = strlen($wanted_re);
            while ($startOffset < $length) {
                $start = strpos($wanted_re, $r, $startOffset);
                if ($start !== false) {
                    // we have found a start tag
                    $end = strpos($wanted_re, $r, $start + 2);
                    if ($end === false) {
                        // unbalanced tag, ignore it.
                        $end = $start = $length;
                    }
                } else {
                    // no more %r sections
                    $start = $end = $length;
                }
                // quote a non re portion of the string
                $temp .= preg_quote(substr($wanted_re, $startOffset, $start - $startOffset), '/');
                // add the re unquoted.
                if ($end > $start) {
                    $temp .= '(' . substr($wanted_re, $start + 2, $end - $start - 2) . ')';
                }
                $startOffset = $end + 2;
            }
            $wanted_re = $temp;

            // Stick to basics
            $wanted_re = str_replace('%e', '\\' . DIRECTORY_SEPARATOR, $wanted_re);
            $wanted_re = str_replace('%s', '[^\r\n]+', $wanted_re);
            $wanted_re = str_replace('%S', '[^\r\n]*', $wanted_re);
            $wanted_re = str_replace('%a', '.+', $wanted_re);
            $wanted_re = str_replace('%A', '.*', $wanted_re);
            $wanted_re = str_replace('%w', '\s*', $wanted_re);
            $wanted_re = str_replace('%i', '[+-]?\d+', $wanted_re);
            $wanted_re = str_replace('%d', '\d+', $wanted_re);
            $wanted_re = str_replace('%x', '[0-9a-fA-F]+', $wanted_re);
            $wanted_re = str_replace('%f', '[+-]?\.?\d+\.?\d*(?:[Ee][+-]?\d+)?', $wanted_re);
            $wanted_re = str_replace('%c', '.', $wanted_re);
            // %f allows two points "-.0.0" but that is the best *simple* expression
        }

        if (preg_match("/^$wanted_re\$/s", $output)) {
            $passed = true;
            if (!$cfg['keep']['php']) {
                @unlink($test_file);
            }
            @unlink($tmp_post);

            if (!$leaked && !$failed_headers) {
                if (isset($section_text['XFAIL'])) {
                    $warn = true;
                    $info = " (warn: XFAIL section but test passes)";
                } elseif (isset($section_text['XLEAK'])) {
                    $warn = true;
                    $info = " (warn: XLEAK section but test passes)";
                } else {
                    show_result("PASS", $tested, $tested_file, '', $temp_filenames);
                    junit_mark_test_as('PASS', $shortname, $tested);
                    return 'PASSED';
                }
            }
        }
    } else {
        $wanted = trim($section_text['EXPECT']);
        $wanted = preg_replace('/\r\n/', "\n", $wanted);
        show_file_block('exp', $wanted);

        // compare and leave on success
        if (!strcmp($output, $wanted)) {
            $passed = true;

            if (!$cfg['keep']['php']) {
                @unlink($test_file);
            }
            @unlink($tmp_post);

            if (!$leaked && !$failed_headers) {
                if (isset($section_text['XFAIL'])) {
                    $warn = true;
                    $info = " (warn: XFAIL section but test passes)";
                } elseif (isset($section_text['XLEAK'])) {
                    $warn = true;
                    $info = " (warn: XLEAK section but test passes)";
                } else {
                    show_result("PASS", $tested, $tested_file, '', $temp_filenames);
                    junit_mark_test_as('PASS', $shortname, $tested);
                    return 'PASSED';
                }
            }
        }

        $wanted_re = null;
    }

    // Test failed so we need to report details.
    if ($failed_headers) {
        $passed = false;
        $wanted = $wanted_headers . "\n--HEADERS--\n" . $wanted;
        $output = $output_headers . "\n--HEADERS--\n" . $output;

        if (isset($wanted_re)) {
            $wanted_re = preg_quote($wanted_headers . "\n--HEADERS--\n", '/') . $wanted_re;
        }
    }

    if ($leaked) {
        $restype[] = isset($section_text['XLEAK']) ?
                        'XLEAK' : 'LEAK';
    }

    if ($warn) {
        $restype[] = 'WARN';
    }

    if (!$passed) {
        if (isset($section_text['XFAIL'])) {
            $restype[] = 'XFAIL';
            $info = '  XFAIL REASON: ' . rtrim($section_text['XFAIL']);
        } elseif (isset($section_text['XLEAK'])) {
            $restype[] = 'XLEAK';
            $info = '  XLEAK REASON: ' . rtrim($section_text['XLEAK']);
        } else {
            $restype[] = 'FAIL';
        }
    }

    if (!$passed) {
        // write .exp
        if (strpos($log_format, 'E') !== false && file_put_contents($exp_filename, $wanted) === false) {
            error("Cannot create expected test output - $exp_filename");
        }

        // write .out
        if (strpos($log_format, 'O') !== false && file_put_contents($output_filename, $output) === false) {
            error("Cannot create test output - $output_filename");
        }

        // write .diff
        $diff = generate_diff($wanted, $wanted_re, $output);
        if (is_array($IN_REDIRECT)) {
            $orig_shortname = str_replace(TEST_PHP_SRCDIR . '/', '', $file);
            $diff = "# original source file: $orig_shortname\n" . $diff;
        }
        show_file_block('diff', $diff);
        if (strpos($log_format, 'D') !== false && file_put_contents($diff_filename, $diff) === false) {
            error("Cannot create test diff - $diff_filename");
        }

        // write .sh
        if (strpos($log_format, 'S') !== false) {
            $env_lines = [];
            foreach ($env as $env_var => $env_val) {
                $env_lines[] = "export $env_var=" . escapeshellarg($env_val ?? "");
            }
            $exported_environment = $env_lines ? "\n" . implode("\n", $env_lines) . "\n" : "";
            $sh_script = <<<SH
#!/bin/sh
{$exported_environment}
case "$1" in
"gdb")
    gdb --args {$cmd}
    ;;
"valgrind")
    USE_ZEND_ALLOC=0 valgrind $2 ${cmd}
    ;;
"rr")
    rr record $2 ${cmd}
    ;;
*)
    {$cmd}
    ;;
esac
SH;
            if (file_put_contents($sh_filename, $sh_script) === false) {
                error("Cannot create test shell script - $sh_filename");
            }
            chmod($sh_filename, 0755);
        }

        // write .log
        if (strpos($log_format, 'L') !== false && file_put_contents($log_filename, "
---- EXPECTED OUTPUT
$wanted
---- ACTUAL OUTPUT
$output
---- FAILED
") === false) {
            error("Cannot create test log - $log_filename");
            error_report($file, $log_filename, $tested);
        }
    }

    if ($valgrind && $leaked && $cfg["show"]["mem"]) {
        show_file_block('mem', file_get_contents($memcheck_filename));
    }

    show_result(implode('&', $restype), $tested, $tested_file, $info, $temp_filenames);

    foreach ($restype as $type) {
        $PHP_FAILED_TESTS[$type . 'ED'][] = [
            'name' => $file,
            'test_name' => (is_array($IN_REDIRECT) ? $IN_REDIRECT['via'] : '') . $tested . " [$tested_file]",
            'output' => $output_filename,
            'diff' => $diff_filename,
            'info' => $info,
        ];
    }

    $diff = empty($diff) ? '' : preg_replace('/\e/', '<esc>', $diff);

    junit_mark_test_as($restype, $shortname, $tested, null, $info, $diff);

    return $restype[0] . 'ED';
}

/**
 * @return bool|int
 */
function comp_line(string $l1, string $l2, bool $is_reg)
{
    if ($is_reg) {
        return preg_match('/^' . $l1 . '$/s', $l2);
    } else {
        return !strcmp($l1, $l2);
    }
}

function count_array_diff(
    array $ar1,
    array $ar2,
    bool $is_reg,
    array $w,
    int $idx1,
    int $idx2,
    int $cnt1,
    int $cnt2,
    int $steps
): int {
    $equal = 0;

    while ($idx1 < $cnt1 && $idx2 < $cnt2 && comp_line($ar1[$idx1], $ar2[$idx2], $is_reg)) {
        $idx1++;
        $idx2++;
        $equal++;
        $steps--;
    }
    if (--$steps > 0) {
        $eq1 = 0;
        $st = $steps / 2;

        for ($ofs1 = $idx1 + 1; $ofs1 < $cnt1 && $st-- > 0; $ofs1++) {
            $eq = @count_array_diff($ar1, $ar2, $is_reg, $w, $ofs1, $idx2, $cnt1, $cnt2, $st);

            if ($eq > $eq1) {
                $eq1 = $eq;
            }
        }

        $eq2 = 0;
        $st = $steps;

        for ($ofs2 = $idx2 + 1; $ofs2 < $cnt2 && $st-- > 0; $ofs2++) {
            $eq = @count_array_diff($ar1, $ar2, $is_reg, $w, $idx1, $ofs2, $cnt1, $cnt2, $st);
            if ($eq > $eq2) {
                $eq2 = $eq;
            }
        }

        if ($eq1 > $eq2) {
            $equal += $eq1;
        } elseif ($eq2 > 0) {
            $equal += $eq2;
        }
    }

    return $equal;
}

function generate_array_diff(array $ar1, array $ar2, bool $is_reg, array $w): array
{
    global $context_line_count;
    $idx1 = 0;
    $cnt1 = @count($ar1);
    $idx2 = 0;
    $cnt2 = @count($ar2);
    $diff = [];
    $old1 = [];
    $old2 = [];
    $number_len = max(3, strlen((string)max($cnt1 + 1, $cnt2 + 1)));
    $line_number_spec = '%0' . $number_len . 'd';

    /** Mapping from $idx2 to $idx1, including indexes of idx2 that are identical to idx1 as well as entries that don't have matches */
    $mapping = [];

    while ($idx1 < $cnt1 && $idx2 < $cnt2) {
        $mapping[$idx2] = $idx1;
        if (comp_line($ar1[$idx1], $ar2[$idx2], $is_reg)) {
            $idx1++;
            $idx2++;
            continue;
        } else {
            $c1 = @count_array_diff($ar1, $ar2, $is_reg, $w, $idx1 + 1, $idx2, $cnt1, $cnt2, 10);
            $c2 = @count_array_diff($ar1, $ar2, $is_reg, $w, $idx1, $idx2 + 1, $cnt1, $cnt2, 10);

            if ($c1 > $c2) {
                $old1[$idx1] = sprintf("{$line_number_spec}- ", $idx1 + 1) . $w[$idx1++];
            } elseif ($c2 > 0) {
                $old2[$idx2] = sprintf("{$line_number_spec}+ ", $idx2 + 1) . $ar2[$idx2++];
            } else {
                $old1[$idx1] = sprintf("{$line_number_spec}- ", $idx1 + 1) . $w[$idx1++];
                $old2[$idx2] = sprintf("{$line_number_spec}+ ", $idx2 + 1) . $ar2[$idx2++];
            }
            $last_printed_context_line = $idx1;
        }
    }
    $mapping[$idx2] = $idx1;

    reset($old1);
    $k1 = key($old1);
    $l1 = -2;
    reset($old2);
    $k2 = key($old2);
    $l2 = -2;
    $old_k1 = -1;
    $add_context_lines = function (int $new_k1) use (&$old_k1, &$diff, $w, $context_line_count, $number_len) {
        if ($old_k1 >= $new_k1 || !$context_line_count) {
            return;
        }
        $end = $new_k1 - 1;
        $range_end = min($end, $old_k1 + $context_line_count);
        if ($old_k1 >= 0) {
            while ($old_k1 < $range_end) {
                $diff[] = str_repeat(' ', $number_len + 2) . $w[$old_k1++];
            }
        }
        if ($end - $context_line_count > $old_k1) {
            $old_k1 = $end - $context_line_count;
            if ($old_k1 > 0) {
                // Add a '--' to mark sections where the common areas were truncated
                $diff[] = '--';
            }
        }
        $old_k1 = max($old_k1, 0);
        while ($old_k1 < $end) {
            $diff[] = str_repeat(' ', $number_len + 2) . $w[$old_k1++];
        }
        $old_k1 = $new_k1;
    };

    while ($k1 !== null || $k2 !== null) {
        if ($k1 == $l1 + 1 || $k2 === null) {
            $add_context_lines($k1);
            $l1 = $k1;
            $diff[] = current($old1);
            $old_k1 = $k1;
            $k1 = next($old1) ? key($old1) : null;
        } elseif ($k2 == $l2 + 1 || $k1 === null) {
            $add_context_lines($mapping[$k2]);
            $l2 = $k2;
            $diff[] = current($old2);
            $k2 = next($old2) ? key($old2) : null;
        } elseif ($k1 < $mapping[$k2]) {
            $add_context_lines($k1);
            $l1 = $k1;
            $diff[] = current($old1);
            $k1 = next($old1) ? key($old1) : null;
        } else {
            $add_context_lines($mapping[$k2]);
            $l2 = $k2;
            $diff[] = current($old2);
            $k2 = next($old2) ? key($old2) : null;
        }
    }

    while ($idx1 < $cnt1) {
        $add_context_lines($idx1 + 1);
        $diff[] = sprintf("{$line_number_spec}- ", $idx1 + 1) . $w[$idx1++];
    }

    while ($idx2 < $cnt2) {
        if (isset($mapping[$idx2])) {
            $add_context_lines($mapping[$idx2] + 1);
        }
        $diff[] = sprintf("{$line_number_spec}+ ", $idx2 + 1) . $ar2[$idx2++];
    }
    $add_context_lines(min($old_k1 + $context_line_count + 1, $cnt1 + 1));
    if ($context_line_count && $old_k1 < $cnt1 + 1) {
        // Add a '--' to mark sections where the common areas were truncated
        $diff[] = '--';
    }

    return $diff;
}

function generate_diff(string $wanted, ?string $wanted_re, string $output): string
{
    $w = explode("\n", $wanted);
    $o = explode("\n", $output);
    $r = is_null($wanted_re) ? $w : explode("\n", $wanted_re);
    $diff = generate_array_diff($r, $o, !is_null($wanted_re), $w);

    return implode(PHP_EOL, $diff);
}

function error(string $message): void
{
    echo "ERROR: {$message}\n";
    exit(1);
}

function settings2array(array $settings, &$ini_settings): void
{
    foreach ($settings as $setting) {
        if (strpos($setting, '=') !== false) {
            $setting = explode("=", $setting, 2);
            $name = trim($setting[0]);
            $value = trim($setting[1]);

            if ($name == 'extension' || $name == 'zend_extension') {
                if (!isset($ini_settings[$name])) {
                    $ini_settings[$name] = [];
                }

                $ini_settings[$name][] = $value;
            } else {
                $ini_settings[$name] = $value;
            }
        }
    }
}

function settings2params(array $ini_settings): string
{
    $settings = '';

    foreach ($ini_settings as $name => $value) {
        if (is_array($value)) {
            foreach ($value as $val) {
                $val = addslashes($val);
                $settings .= " -d \"$name=$val\"";
            }
        } else {
            if (IS_WINDOWS && !empty($value) && $value[0] == '"') {
                $len = strlen($value);

                if ($value[$len - 1] == '"') {
                    $value[0] = "'";
                    $value[$len - 1] = "'";
                }
            } else {
                $value = addslashes($value);
            }

            $settings .= " -d \"$name=$value\"";
        }
    }

    return $settings;
}

function compute_summary(): void
{
    global $n_total, $test_results, $ignored_by_ext, $sum_results, $percent_results;

    $n_total = count($test_results);
    $n_total += $ignored_by_ext;
    $sum_results = [
        'PASSED' => 0,
        'WARNED' => 0,
        'SKIPPED' => 0,
        'FAILED' => 0,
        'BORKED' => 0,
        'LEAKED' => 0,
        'XFAILED' => 0,
        'XLEAKED' => 0
    ];

    foreach ($test_results as $v) {
        $sum_results[$v]++;
    }

    $sum_results['SKIPPED'] += $ignored_by_ext;
    $percent_results = [];

    foreach ($sum_results as $v => $n) {
        $percent_results[$v] = (100.0 * $n) / $n_total;
    }
}

function get_summary(bool $show_ext_summary): string
{
    global $exts_skipped, $exts_tested, $n_total, $sum_results, $percent_results, $end_time, $start_time, $failed_test_summary, $PHP_FAILED_TESTS, $valgrind;

    $x_total = $n_total - $sum_results['SKIPPED'] - $sum_results['BORKED'];

    if ($x_total) {
        $x_warned = (100.0 * $sum_results['WARNED']) / $x_total;
        $x_failed = (100.0 * $sum_results['FAILED']) / $x_total;
        $x_xfailed = (100.0 * $sum_results['XFAILED']) / $x_total;
        $x_xleaked = (100.0 * $sum_results['XLEAKED']) / $x_total;
        $x_leaked = (100.0 * $sum_results['LEAKED']) / $x_total;
        $x_passed = (100.0 * $sum_results['PASSED']) / $x_total;
    } else {
        $x_warned = $x_failed = $x_passed = $x_leaked = $x_xfailed = $x_xleaked = 0;
    }

    $summary = '';

    if ($show_ext_summary) {
        $summary .= '
=====================================================================
TEST RESULT SUMMARY
---------------------------------------------------------------------
Exts skipped    : ' . sprintf('%4d', $exts_skipped) . '
Exts tested     : ' . sprintf('%4d', $exts_tested) . '
---------------------------------------------------------------------
';
    }

    $summary .= '
Number of tests : ' . sprintf('%4d', $n_total) . '          ' . sprintf('%8d', $x_total);

    if ($sum_results['BORKED']) {
        $summary .= '
Tests borked    : ' . sprintf('%4d (%5.1f%%)', $sum_results['BORKED'], $percent_results['BORKED']) . ' --------';
    }

    $summary .= '
Tests skipped   : ' . sprintf('%4d (%5.1f%%)', $sum_results['SKIPPED'], $percent_results['SKIPPED']) . ' --------
Tests warned    : ' . sprintf('%4d (%5.1f%%)', $sum_results['WARNED'], $percent_results['WARNED']) . ' ' . sprintf('(%5.1f%%)', $x_warned) . '
Tests failed    : ' . sprintf('%4d (%5.1f%%)', $sum_results['FAILED'], $percent_results['FAILED']) . ' ' . sprintf('(%5.1f%%)', $x_failed);

    if ($sum_results['XFAILED']) {
        $summary .= '
Expected fail   : ' . sprintf('%4d (%5.1f%%)', $sum_results['XFAILED'], $percent_results['XFAILED']) . ' ' . sprintf('(%5.1f%%)', $x_xfailed);
    }

    if ($valgrind) {
        $summary .= '
Tests leaked    : ' . sprintf('%4d (%5.1f%%)', $sum_results['LEAKED'], $percent_results['LEAKED']) . ' ' . sprintf('(%5.1f%%)', $x_leaked);
        if ($sum_results['XLEAKED']) {
            $summary .= '
Expected leak   : ' . sprintf('%4d (%5.1f%%)', $sum_results['XLEAKED'], $percent_results['XLEAKED']) . ' ' . sprintf('(%5.1f%%)', $x_xleaked);
        }
    }

    $summary .= '
Tests passed    : ' . sprintf('%4d (%5.1f%%)', $sum_results['PASSED'], $percent_results['PASSED']) . ' ' . sprintf('(%5.1f%%)', $x_passed) . '
---------------------------------------------------------------------
Time taken      : ' . sprintf('%4d seconds', $end_time - $start_time) . '
=====================================================================
';
    $failed_test_summary = '';

    if (count($PHP_FAILED_TESTS['SLOW'])) {
        usort($PHP_FAILED_TESTS['SLOW'], function (array $a, array $b): int {
            return $a['info'] < $b['info'] ? 1 : -1;
        });

        $failed_test_summary .= '
=====================================================================
SLOW TEST SUMMARY
---------------------------------------------------------------------
';
        foreach ($PHP_FAILED_TESTS['SLOW'] as $failed_test_data) {
            $failed_test_summary .= sprintf('(%.3f s) ', $failed_test_data['info']) . $failed_test_data['test_name'] . "\n";
        }
        $failed_test_summary .= "=====================================================================\n";
    }

    if (count($PHP_FAILED_TESTS['XFAILED'])) {
        $failed_test_summary .= '
=====================================================================
EXPECTED FAILED TEST SUMMARY
---------------------------------------------------------------------
';
        foreach ($PHP_FAILED_TESTS['XFAILED'] as $failed_test_data) {
            $failed_test_summary .= $failed_test_data['test_name'] . $failed_test_data['info'] . "\n";
        }
        $failed_test_summary .= "=====================================================================\n";
    }

    if (count($PHP_FAILED_TESTS['BORKED'])) {
        $failed_test_summary .= '
=====================================================================
BORKED TEST SUMMARY
---------------------------------------------------------------------
';
        foreach ($PHP_FAILED_TESTS['BORKED'] as $failed_test_data) {
            $failed_test_summary .= $failed_test_data['info'] . "\n";
        }

        $failed_test_summary .= "=====================================================================\n";
    }

    if (count($PHP_FAILED_TESTS['FAILED'])) {
        $failed_test_summary .= '
=====================================================================
FAILED TEST SUMMARY
---------------------------------------------------------------------
';
        foreach ($PHP_FAILED_TESTS['FAILED'] as $failed_test_data) {
            $failed_test_summary .= $failed_test_data['test_name'] . $failed_test_data['info'] . "\n";
        }
        $failed_test_summary .= "=====================================================================\n";
    }
    if (count($PHP_FAILED_TESTS['WARNED'])) {
        $failed_test_summary .= '
=====================================================================
WARNED TEST SUMMARY
---------------------------------------------------------------------
';
        foreach ($PHP_FAILED_TESTS['WARNED'] as $failed_test_data) {
            $failed_test_summary .= $failed_test_data['test_name'] . $failed_test_data['info'] . "\n";
        }

        $failed_test_summary .= "=====================================================================\n";
    }

    if (count($PHP_FAILED_TESTS['LEAKED'])) {
        $failed_test_summary .= '
=====================================================================
LEAKED TEST SUMMARY
---------------------------------------------------------------------
';
        foreach ($PHP_FAILED_TESTS['LEAKED'] as $failed_test_data) {
            $failed_test_summary .= $failed_test_data['test_name'] . $failed_test_data['info'] . "\n";
        }

        $failed_test_summary .= "=====================================================================\n";
    }

    if (count($PHP_FAILED_TESTS['XLEAKED'])) {
        $failed_test_summary .= '
=====================================================================
EXPECTED LEAK TEST SUMMARY
---------------------------------------------------------------------
';
        foreach ($PHP_FAILED_TESTS['XLEAKED'] as $failed_test_data) {
            $failed_test_summary .= $failed_test_data['test_name'] . $failed_test_data['info'] . "\n";
        }

        $failed_test_summary .= "=====================================================================\n";
    }

    if ($failed_test_summary && !getenv('NO_PHPTEST_SUMMARY')) {
        $summary .= $failed_test_summary;
    }

    return $summary;
}

function show_start($start_time): void
{
    echo "TIME START " . date('Y-m-d H:i:s', $start_time) . "\n=====================================================================\n";
}

function show_end($end_time): void
{
    echo "=====================================================================\nTIME END " . date('Y-m-d H:i:s', $end_time) . "\n";
}

function show_summary(): void
{
    echo get_summary(true);
}

function show_redirect_start(string $tests, string $tested, string $tested_file): void
{
    global $SHOW_ONLY_GROUPS;

    if (!$SHOW_ONLY_GROUPS || in_array('REDIRECT', $SHOW_ONLY_GROUPS)) {
        echo "REDIRECT $tests ($tested [$tested_file]) begin\n";
    } else {
        clear_show_test();
    }
}

function show_redirect_ends(string $tests, string $tested, string $tested_file): void
{
    global $SHOW_ONLY_GROUPS;

    if (!$SHOW_ONLY_GROUPS || in_array('REDIRECT', $SHOW_ONLY_GROUPS)) {
        echo "REDIRECT $tests ($tested [$tested_file]) done\n";
    } else {
        clear_show_test();
    }
}

function show_test(int $test_idx, string $shortname): void
{
    global $test_cnt;
    global $line_length;

    $str = "TEST $test_idx/$test_cnt [$shortname]\r";
    $line_length = strlen($str);
    echo $str;
    flush();
}

function clear_show_test(): void
{
    global $line_length;
    // Parallel testing
    global $workerID;

    if (!$workerID) {
        // Write over the last line to avoid random trailing chars on next echo
        echo str_repeat(" ", $line_length), "\r";
    }
}

function parse_conflicts(string $text): array
{
    // Strip comments
    $text = preg_replace('/#.*/', '', $text);
    return array_map('trim', explode("\n", trim($text)));
}

function show_result(
    string $result,
    string $tested,
    string $tested_file,
    string $extra = '',
    ?array $temp_filenames = null
): void {
    global $SHOW_ONLY_GROUPS, $colorize;

    if (!$SHOW_ONLY_GROUPS || in_array($result, $SHOW_ONLY_GROUPS)) {
        if ($colorize) {
            /* Use ANSI escape codes for coloring test result */
            switch ( $result ) {
                case 'PASS': // Light Green
                    $color = "\e[1;32m{$result}\e[0m"; break;
                case 'FAIL':
                case 'BORK':
                case 'LEAK':
                case 'LEAK&FAIL':
                    // Light Red
                    $color = "\e[1;31m{$result}\e[0m"; break;
                default: // Yellow
                    $color = "\e[1;33m{$result}\e[0m"; break;
            }

            echo "$color $tested [$tested_file] $extra\n";
        } else {
            echo "$result $tested [$tested_file] $extra\n";
        }
    } elseif (!$SHOW_ONLY_GROUPS) {
        clear_show_test();
    }

}

function junit_init(): void
{
    // Check whether a junit log is wanted.
    global $workerID;
    $JUNIT = getenv('TEST_PHP_JUNIT');
    if (empty($JUNIT)) {
        $GLOBALS['JUNIT'] = false;
        return;
    }
    if ($workerID) {
        $fp = null;
    } elseif (!$fp = fopen($JUNIT, 'w')) {
        error("Failed to open $JUNIT for writing.");
    }
    $GLOBALS['JUNIT'] = [
        'fp' => $fp,
        'name' => 'PHP',
        'test_total' => 0,
        'test_pass' => 0,
        'test_fail' => 0,
        'test_error' => 0,
        'test_skip' => 0,
        'test_warn' => 0,
        'execution_time' => 0,
        'suites' => [],
        'files' => []
    ];
}

function junit_save_xml(): void
{
    global $JUNIT;
    if (!junit_enabled()) {
        return;
    }

    $xml = '<' . '?' . 'xml version="1.0" encoding="UTF-8"' . '?' . '>' . PHP_EOL;
    $xml .= sprintf(
        '<testsuites name="%s" tests="%s" failures="%d" errors="%d" skip="%d" time="%s">' . PHP_EOL,
        $JUNIT['name'],
        $JUNIT['test_total'],
        $JUNIT['test_fail'],
        $JUNIT['test_error'],
        $JUNIT['test_skip'],
        $JUNIT['execution_time']
    );
    $xml .= junit_get_suite_xml();
    $xml .= '</testsuites>';
    fwrite($JUNIT['fp'], $xml);
}

function junit_get_suite_xml(string $suite_name = ''): string
{
    global $JUNIT;

    $result = "";

    foreach ($JUNIT['suites'] as $suite_name => $suite) {
        $result .= sprintf(
            '<testsuite name="%s" tests="%s" failures="%d" errors="%d" skip="%d" time="%s">' . PHP_EOL,
            $suite['name'],
            $suite['test_total'],
            $suite['test_fail'],
            $suite['test_error'],
            $suite['test_skip'],
            $suite['execution_time']
        );

        if (!empty($suite_name)) {
            foreach ($suite['files'] as $file) {
                $result .= $JUNIT['files'][$file]['xml'];
            }
        }

        $result .= '</testsuite>' . PHP_EOL;
    }

    return $result;
}

function junit_enabled(): bool
{
    global $JUNIT;
    return !empty($JUNIT);
}

/**
 * @param array|string $type
 */
function junit_mark_test_as(
    $type,
    string $file_name,
    string $test_name,
    ?float $time = null,
    string $message = '',
    string $details = ''
): void {
    global $JUNIT;
    if (!junit_enabled()) {
        return;
    }

    $suite = junit_get_suitename_for($file_name);

    junit_suite_record($suite, 'test_total');

    $time = $time ?? junit_get_timer($file_name);
    junit_suite_record($suite, 'execution_time', $time);

    $escaped_details = htmlspecialchars($details, ENT_QUOTES, 'UTF-8');
    $escaped_details = preg_replace_callback('/[\0-\x08\x0B\x0C\x0E-\x1F]/', function (array $c): string {
        return sprintf('[[0x%02x]]', ord($c[0]));
    }, $escaped_details);
    $escaped_message = htmlspecialchars($message, ENT_QUOTES, 'UTF-8');

    $escaped_test_name = htmlspecialchars($file_name . ' (' . $test_name . ')', ENT_QUOTES);
    $JUNIT['files'][$file_name]['xml'] = "<testcase name='$escaped_test_name' time='$time'>\n";

    if (is_array($type)) {
        $output_type = $type[0] . 'ED';
        $temp = array_intersect(['XFAIL', 'XLEAK', 'FAIL', 'WARN'], $type);
        $type = reset($temp);
    } else {
        $output_type = $type . 'ED';
    }

    if ('PASS' == $type || 'XFAIL' == $type || 'XLEAK' == $type) {
        junit_suite_record($suite, 'test_pass');
    } elseif ('BORK' == $type) {
        junit_suite_record($suite, 'test_error');
        $JUNIT['files'][$file_name]['xml'] .= "<error type='$output_type' message='$escaped_message'/>\n";
    } elseif ('SKIP' == $type) {
        junit_suite_record($suite, 'test_skip');
        $JUNIT['files'][$file_name]['xml'] .= "<skipped>$escaped_message</skipped>\n";
    } elseif ('WARN' == $type) {
        junit_suite_record($suite, 'test_warn');
        $JUNIT['files'][$file_name]['xml'] .= "<warning>$escaped_message</warning>\n";
    } elseif ('FAIL' == $type) {
        junit_suite_record($suite, 'test_fail');
        $JUNIT['files'][$file_name]['xml'] .= "<failure type='$output_type' message='$escaped_message'>$escaped_details</failure>\n";
    } else {
        junit_suite_record($suite, 'test_error');
        $JUNIT['files'][$file_name]['xml'] .= "<error type='$output_type' message='$escaped_message'>$escaped_details</error>\n";
    }

    $JUNIT['files'][$file_name]['xml'] .= "</testcase>\n";
}

function junit_suite_record(string $suite, string $param, float $value = 1): void
{
    global $JUNIT;

    $JUNIT[$param] += $value;
    $JUNIT['suites'][$suite][$param] += $value;
}

function junit_get_timer(string $file_name): float
{
    global $JUNIT;
    if (!junit_enabled()) {
        return 0;
    }

    if (isset($JUNIT['files'][$file_name]['total'])) {
        return number_format($JUNIT['files'][$file_name]['total'], 4);
    }

    return 0;
}

function junit_start_timer(string $file_name): void
{
    global $JUNIT;
    if (!junit_enabled()) {
        return;
    }

    if (!isset($JUNIT['files'][$file_name]['start'])) {
        $JUNIT['files'][$file_name]['start'] = microtime(true);

        $suite = junit_get_suitename_for($file_name);
        junit_init_suite($suite);
        $JUNIT['suites'][$suite]['files'][$file_name] = $file_name;
    }
}

function junit_get_suitename_for(string $file_name): string
{
    return junit_path_to_classname(dirname($file_name));
}

function junit_path_to_classname(string $file_name): string
{
    global $JUNIT;

    if (!junit_enabled()) {
        return '';
    }

    $ret = $JUNIT['name'];
    $_tmp = [];

    // lookup whether we're in the PHP source checkout
    $max = 5;
    if (is_file($file_name)) {
        $dir = dirname(realpath($file_name));
    } else {
        $dir = realpath($file_name);
    }
    do {
        array_unshift($_tmp, basename($dir));
        $chk = $dir . DIRECTORY_SEPARATOR . "main" . DIRECTORY_SEPARATOR . "php_version.h";
        $dir = dirname($dir);
    } while (!file_exists($chk) && --$max > 0);
    if (file_exists($chk)) {
        if ($max) {
            array_shift($_tmp);
        }
        foreach ($_tmp as $p) {
            $ret .= "." . preg_replace(",[^a-z0-9]+,i", ".", $p);
        }
        return $ret;
    }

    return $JUNIT['name'] . '.' . str_replace([DIRECTORY_SEPARATOR, '-'], '.', $file_name);
}

function junit_init_suite(string $suite_name): void
{
    global $JUNIT;
    if (!junit_enabled()) {
        return;
    }

    if (!empty($JUNIT['suites'][$suite_name])) {
        return;
    }

    $JUNIT['suites'][$suite_name] = [
        'name' => $suite_name,
        'test_total' => 0,
        'test_pass' => 0,
        'test_fail' => 0,
        'test_error' => 0,
        'test_skip' => 0,
        'test_warn' => 0,
        'files' => [],
        'execution_time' => 0,
    ];
}

function junit_finish_timer(string $file_name): void
{
    global $JUNIT;
    if (!junit_enabled()) {
        return;
    }

    if (!isset($JUNIT['files'][$file_name]['start'])) {
        error("Timer for $file_name was not started!");
    }

    if (!isset($JUNIT['files'][$file_name]['total'])) {
        $JUNIT['files'][$file_name]['total'] = 0;
    }

    $start = $JUNIT['files'][$file_name]['start'];
    $JUNIT['files'][$file_name]['total'] += microtime(true) - $start;
    unset($JUNIT['files'][$file_name]['start']);
}

function junit_merge_results(array $junit): void
{
    global $JUNIT;
    $JUNIT['test_total'] += $junit['test_total'];
    $JUNIT['test_pass']  += $junit['test_pass'];
    $JUNIT['test_fail']  += $junit['test_fail'];
    $JUNIT['test_error'] += $junit['test_error'];
    $JUNIT['test_skip']  += $junit['test_skip'];
    $JUNIT['test_warn']  += $junit['test_warn'];
    $JUNIT['execution_time'] += $junit['execution_time'];
    $JUNIT['files'] += $junit['files'];
    foreach ($junit['suites'] as $name => $suite) {
        if (!isset($JUNIT['suites'][$name])) {
            $JUNIT['suites'][$name] = $suite;
            continue;
        }

        $SUITE =& $JUNIT['suites'][$name];
        $SUITE['test_total'] += $suite['test_total'];
        $SUITE['test_pass']  += $suite['test_pass'];
        $SUITE['test_fail']  += $suite['test_fail'];
        $SUITE['test_error'] += $suite['test_error'];
        $SUITE['test_skip']  += $suite['test_skip'];
        $SUITE['test_warn']  += $suite['test_warn'];
        $SUITE['execution_time'] += $suite['execution_time'];
        $SUITE['files'] += $suite['files'];
    }
}

class RuntestsValgrind
{
    protected $version = '';
    protected $header = '';
    protected $version_3_3_0 = false;
    protected $version_3_8_0 = false;
    protected $tool = null;

    public function getVersion(): string
    {
        return $this->version;
    }

    public function getHeader(): string
    {
        return $this->header;
    }

    public function __construct(array $environment, string $tool = 'memcheck')
    {
        $this->tool = $tool;
        $header = system_with_timeout("valgrind --tool={$this->tool} --version", $environment);
        if (!$header) {
            error("Valgrind returned no version info for {$this->tool}, cannot proceed.\n".
                  "Please check if Valgrind is installed and the tool is named correctly.");
        }
        $count = 0;
        $version = preg_replace("/valgrind-(\d+)\.(\d+)\.(\d+)([.\w_-]+)?(\s+)/", '$1.$2.$3', $header, 1, $count);
        if ($count != 1) {
            error("Valgrind returned invalid version info (\"{$header}\") for {$this->tool}, cannot proceed.");
        }
        $this->version = $version;
        $this->header = sprintf(
            "%s (%s)", trim($header), $this->tool);
        $this->version_3_3_0 = version_compare($version, '3.3.0', '>=');
        $this->version_3_8_0 = version_compare($version, '3.8.0', '>=');
    }

    public function wrapCommand(string $cmd, string $memcheck_filename, bool $check_all): string
    {
        $vcmd = "valgrind -q --tool={$this->tool} --trace-children=yes";
        if ($check_all) {
            $vcmd .= ' --smc-check=all';
        }

        /* --vex-iropt-register-updates=allregs-at-mem-access is necessary for phpdbg watchpoint tests */
        if ($this->version_3_8_0) {
            /* valgrind 3.3.0+ doesn't have --log-file-exactly option */
            return "$vcmd --vex-iropt-register-updates=allregs-at-mem-access --log-file=$memcheck_filename $cmd";
        } elseif ($this->version_3_3_0) {
            return "$vcmd --vex-iropt-precise-memory-exns=yes --log-file=$memcheck_filename $cmd";
        } else {
            return "$vcmd --vex-iropt-precise-memory-exns=yes --log-file-exactly=$memcheck_filename $cmd";
        }
    }
}

function init_output_buffers(): void
{
    // Delete as much output buffers as possible.
    while (@ob_end_clean()) {
    }

    if (ob_get_level()) {
        echo "Not all buffers were deleted.\n";
    }
}

function check_proc_open_function_exists(): void
{
    if (!function_exists('proc_open')) {
        echo <<<NO_PROC_OPEN_ERROR

+-----------------------------------------------------------+
|                       ! ERROR !                           |
| The test-suite requires that proc_open() is available.    |
| Please check if you disabled it in php.ini.               |
+-----------------------------------------------------------+

NO_PROC_OPEN_ERROR;
        exit(1);
    }
}

main();
