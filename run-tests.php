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

/* Let there be no top-level code beyond this point:
 * Only functions and classes, thanks!
 *
 * Minimum required PHP version: 7.4.0
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

    --progress
    --no-progress  Do/Don't show the current progress.

    --repeat [n]
                Run the tests multiple times in the same process and check the
                output of the last execution (CLI SAPI only).

    --bless     Bless failed tests using scripts/dev/bless_tests.php.

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
           $end_time, $environment,
           $exts_skipped, $exts_tested, $exts_to_test, $failed_tests_file,
           $ignored_by_ext, $ini_overwrites, $colorize,
           $log_format, $no_clean, $no_file_cache,
           $pass_options, $php, $php_cgi, $preload,
           $result_tests_file, $slow_min_ms, $start_time,
           $temp_source, $temp_target, $test_cnt,
           $test_files, $test_idx, $test_results, $testfile,
           $valgrind, $sum_results, $shuffle, $file_cache, $num_repeats,
           $show_progress;
    // Parallel testing
    global $workers, $workerID;
    global $context_line_count;

    // Temporary for the duration of refactoring
    /** @var JUnit $junit */
    global $junit;

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
        }

        if (count($environment) == 1) {
            // Not having other environment variables, only having TEMP, is
            // probably ok, but strange and may make a difference in the
            // test pass rate, so warn the user.
            echo "WARNING: Only 1 environment variable will be available to tests(TEMP environment variable)" , PHP_EOL;
        }
    }

    if (IS_WINDOWS && empty($environment["SystemRoot"])) {
        $environment["SystemRoot"] = getenv("SystemRoot");
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

    $junit = new JUnit($environment, $workerID);

    if (getenv('SHOW_ONLY_GROUPS')) {
        $SHOW_ONLY_GROUPS = explode(",", getenv('SHOW_ONLY_GROUPS'));
    } else {
        $SHOW_ONLY_GROUPS = [];
    }

    // Check whether user test dirs are requested.
    $user_tests = [];
    if (getenv('TEST_PHP_USER')) {
        $user_tests = explode(',', getenv('TEST_PHP_USER'));
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
        'opcache.fast_shutdown=0',
        'opcache.file_update_protection=0',
        'opcache.revalidate_freq=0',
        'opcache.jit_hot_loop=1',
        'opcache.jit_hot_func=1',
        'opcache.jit_hot_return=1',
        'opcache.jit_hot_side_exit=1',
        'opcache.jit_max_root_traces=100000',
        'opcache.jit_max_side_traces=100000',
        'opcache.jit_max_exit_counters=100000',
        'opcache.protect_memory=1',
        'zend.assertions=1',
        'zend.exception_ignore_args=0',
        'zend.exception_string_param_max_len=15',
        'short_open_tag=0',
    ];

    $no_file_cache = '-d opcache.file_cache= -d opcache.file_cache_only=0';

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
    if (array_key_exists('NO_COLOR', $environment)) {
        $colorize = false;
    }
    $selected_tests = false;
    $slow_min_ms = INF;
    $preload = false;
    $file_cache = null;
    $shuffle = false;
    $bless = false;
    $workers = null;
    $context_line_count = 3;
    $num_repeats = 1;
    $show_progress = true;
    $ignored_by_ext = [];

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

            if (!$is_switch) {
                $is_switch = true;
                break;
            }

            $repeat = false;

            switch ($switch) {
                case 'j':
                    $workers = substr($argv[$i], 2);
                    if ($workers == 0 || !preg_match('/^\d+$/', $workers)) {
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
                            } elseif (strlen($test)) {
                                $test_files[] = trim($test);
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
                case '--keep-all':
                    foreach ($cfgfiles as $file) {
                        $cfg['keep'][$file] = true;
                    }
                    break;
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
                    $environment['SKIP_PRELOAD'] = 1;
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
                        $environment['MSAN_OPTIONS'] = 'intercept_tls_get_addr=0';
                    }

                    $lsanSuppressions = __DIR__ . '/.github/lsan-suppressions.txt';
                    if (file_exists($lsanSuppressions)) {
                        $environment['LSAN_OPTIONS'] = 'suppressions=' . $lsanSuppressions
                            . ':print_suppressions=0';
                    }
                    break;
                case '--repeat':
                    $num_repeats = (int) $argv[++$i];
                    $environment['SKIP_REPEAT'] = 1;
                    break;
                case '--bless':
                    $bless = true;
                    break;
                case '-':
                    // repeat check with full switch
                    $switch = $argv[$i];
                    if ($switch != '-') {
                        $repeat = true;
                    }
                    break;
                case '--progress':
                    $show_progress = true;
                    break;
                case '--no-progress':
                    $show_progress = false;
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
                } elseif (preg_match("/\*$/", $argv[$i])) {
                    $pattern_match = glob($argv[$i] . '.phpt');
                } else {
                    die('Cannot find test file "' . $argv[$i] . '".' . PHP_EOL);
                }

                if (is_array($pattern_match)) {
                    $test_files = array_merge($test_files, $pattern_match);
                }
            } elseif (is_dir($testfile)) {
                find_files($testfile);
            } elseif (substr($testfile, -5) == '.phpt') {
                $test_files[] = $testfile;
            } else {
                die('Cannot find test file "' . $argv[$i] . '".' . PHP_EOL);
            }
        }
    }

    if ($selected_tests && count($test_files) === 0) {
        echo "No tests found.\n";
        return;
    }

    if (!$php) {
        $php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
    }

    $php_cgi = getenv('TEST_PHP_CGI_EXECUTABLE') ?: get_binary($php, 'php-cgi', 'sapi/cgi/php-cgi');
    $phpdbg = getenv('TEST_PHPDBG_EXECUTABLE') ?: get_binary($php, 'phpdbg', 'sapi/phpdbg/phpdbg');

    putenv("TEST_PHP_EXECUTABLE=$php");
    $environment['TEST_PHP_EXECUTABLE'] = $php;
    putenv("TEST_PHP_EXECUTABLE_ESCAPED=" . escapeshellarg($php));
    $environment['TEST_PHP_EXECUTABLE_ESCAPED'] = escapeshellarg($php);
    putenv("TEST_PHP_CGI_EXECUTABLE=$php_cgi");
    $environment['TEST_PHP_CGI_EXECUTABLE'] = $php_cgi;
    putenv("TEST_PHP_CGI_EXECUTABLE_ESCAPED=" . escapeshellarg($php_cgi ?? ''));
    $environment['TEST_PHP_CGI_EXECUTABLE_ESCAPED'] = escapeshellarg($php_cgi ?? '');
    putenv("TEST_PHPDBG_EXECUTABLE=$phpdbg");
    $environment['TEST_PHPDBG_EXECUTABLE'] = $phpdbg;
    putenv("TEST_PHPDBG_EXECUTABLE_ESCAPED=" . escapeshellarg($phpdbg ?? ''));
    $environment['TEST_PHPDBG_EXECUTABLE_ESCAPED'] = escapeshellarg($phpdbg ?? '');

    if ($conf_passed !== null) {
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

    verify_config($php);
    write_information($user_tests, $phpdbg);

    if ($test_cnt) {
        putenv('NO_INTERACTION=1');
        usort($test_files, "test_sort");
        $start_timestamp = time();
        $start_time = hrtime(true);

        echo "Running selected tests.\n";

        $test_idx = 0;
        run_all_tests($test_files, $environment);
        $end_time = hrtime(true);

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
            save_results($output_file, /* prompt_to_save_results: */ false);
        }
    } else {
        // Compile a list of all test files (*.phpt).
        $test_files = [];
        $exts_tested = $exts_to_test;
        $exts_skipped = [];
        sort($exts_to_test);

        foreach (['Zend', 'tests', 'ext', 'sapi'] as $dir) {
            if (is_dir($dir)) {
                find_files(TEST_PHP_SRCDIR . "/{$dir}", $dir == 'ext');
            }
        }

        foreach ($user_tests as $dir) {
            find_files($dir, $dir == 'ext');
        }

        $test_files = array_unique($test_files);
        usort($test_files, "test_sort");

        $start_timestamp = time();
        $start_time = hrtime(true);
        show_start($start_timestamp);

        $test_cnt = count($test_files);
        $test_idx = 0;
        run_all_tests($test_files, $environment);
        $end_time = hrtime(true);

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

        show_end($start_timestamp, $start_time, $end_time);
        show_summary();

        save_results($output_file, /* prompt_to_save_results: */ true);
    }

    $junit->saveXML();
    if ($bless) {
        bless_failed_tests($PHP_FAILED_TESTS['FAILED']);
    }
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

function verify_config(string $php): void
{
    if (empty($php) || !file_exists($php)) {
        error('environment variable TEST_PHP_EXECUTABLE must be set to specify PHP executable!');
    }

    if (!is_executable($php)) {
        error("invalid PHP executable specified by TEST_PHP_EXECUTABLE  = $php");
    }
}

/**
 * @param string[] $user_tests
 */
function write_information(array $user_tests, $phpdbg): void
{
    global $php, $php_cgi, $php_info, $ini_overwrites, $pass_options, $exts_to_test, $valgrind, $no_file_cache;
    $php_escaped = escapeshellarg($php);

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
    $php_info = shell_exec("$php_escaped $pass_options $info_params $no_file_cache \"$info_file\"");
    define('TESTED_PHP_VERSION', shell_exec("$php_escaped -n -r \"echo PHP_VERSION;\""));

    if ($php_cgi && $php != $php_cgi) {
        $php_cgi_escaped = escapeshellarg($php_cgi);
        $php_info_cgi = shell_exec("$php_cgi_escaped $pass_options $info_params $no_file_cache -q \"$info_file\"");
        $php_info_sep = "\n---------------------------------------------------------------------";
        $php_cgi_info = "$php_info_sep\nPHP         : $php_cgi $php_info_cgi$php_info_sep";
    } else {
        $php_cgi_info = '';
    }

    if ($phpdbg) {
        $phpdbg_escaped = escapeshellarg($phpdbg);
        $phpdbg_info = shell_exec("$phpdbg_escaped $pass_options $info_params $no_file_cache -qrr \"$info_file\"");
        $php_info_sep = "\n---------------------------------------------------------------------";
        $phpdbg_info = "$php_info_sep\nPHP         : $phpdbg $phpdbg_info$php_info_sep";
    } else {
        $phpdbg_info = '';
    }

    if (function_exists('opcache_invalidate')) {
        opcache_invalidate($info_file, true);
    }
    @unlink($info_file);

    // load list of enabled and loadable extensions
    save_text($info_file, <<<'PHP'
        <?php
        $exts = get_loaded_extensions();
        $ext_dir = ini_get('extension_dir');
        foreach (scandir($ext_dir) as $file) {
            if (preg_match('/^(?:php_)?([_a-zA-Z0-9]+)\.(?:so|dll)$/', $file, $matches)) {
                if (!extension_loaded($matches[1]) && @dl($matches[1])) {
                    $exts[] = $matches[1];
                }
            }
        }
        echo implode(',', $exts);
        PHP);
    $extensionsNames = explode(',', shell_exec("$php_escaped $pass_options $info_params $no_file_cache \"$info_file\""));
    $exts_to_test = array_unique(remap_loaded_extensions_names($extensionsNames));
    // check for extensions that need special handling and regenerate
    $info_params_ex = [
        'session' => ['session.auto_start=0'],
        'tidy' => ['tidy.clean_output=0'],
        'zlib' => ['zlib.output_compression=Off'],
        'xdebug' => ['xdebug.mode=off'],
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

function save_results(string $output_file, bool $prompt_to_save_results): void
{
    global $sum_results, $failed_test_summary, $PHP_FAILED_TESTS, $php;

    if (getenv('NO_INTERACTION') || TRAVIS_CI) {
        return;
    }

    if ($prompt_to_save_results) {
        /* We got failed Tests, offer the user to save a QA report */
        $fp = fopen("php://stdin", "r+");
        if ($sum_results['FAILED'] || $sum_results['BORKED'] || $sum_results['WARNED'] || $sum_results['LEAKED']) {
            echo "\nYou may have found a problem in PHP.";
        }
        echo "\nThis report can be saved and used to open an issue on the bug tracker at\n";
        echo "https://github.com/php/php-src/issues\n";
        echo "This gives us a better understanding of PHP's behavior.\n";
        echo "Do you want to save this report in a file? [Yn]: ";
        flush();

        $user_input = fgets($fp, 10);
        fclose($fp);
        if (!(strlen(trim($user_input)) == 0 || strtolower($user_input[0]) == 'y')) {
            return;
        }
    }
    /**
     * Collect information about the host system for our report
     * Fetch phpinfo() output so that we can see the PHP environment
     * Make an archive of all the failed tests
     */
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
    $failed_tests_data .= $sep . "PHPINFO" . $sep;
    $failed_tests_data .= shell_exec($php . ' -ddisplay_errors=stderr -dhtml_errors=0 -i 2> /dev/null');

    file_put_contents($output_file, $failed_tests_data);
    echo "Report saved to: ", $output_file, "\n";
}

function get_binary(string $php, string $sapi, string $sapi_path): ?string
{
    $dir = dirname($php);
    if (IS_WINDOWS && file_exists("$dir/$sapi.exe")) {
        return realpath("$dir/$sapi.exe");
    }
    // Sources tree
    if (file_exists("$dir/../../$sapi_path")) {
        return realpath("$dir/../../$sapi_path");
    }
    // Installation tree, preserve command prefix/suffix
    $inst = str_replace('php', $sapi, basename($php));
    if (file_exists("$dir/$inst")) {
        return realpath("$dir/$inst");
    }
    return null;
}

function find_files(string $dir, bool $is_ext_dir = false, bool $ignore = false): void
{
    global $test_files, $exts_to_test, $ignored_by_ext, $exts_skipped;

    $o = opendir($dir) or error("cannot open directory: $dir");

    while (($name = readdir($o)) !== false) {
        if (is_dir("{$dir}/{$name}") && !in_array($name, ['.', '..', '.svn'])) {
            $skip_ext = ($is_ext_dir && !in_array($name, $exts_to_test));
            if ($skip_ext) {
                $exts_skipped[] = $name;
            }
            find_files("{$dir}/{$name}", false, $ignore || $skip_ext);
        }

        // Cleanup any left-over tmp files from last run.
        if (substr($name, -4) == '.tmp') {
            @unlink("$dir/$name");
            continue;
        }

        // Otherwise we're only interested in *.phpt files.
        // (but not those starting with a dot, which are hidden on
        // many platforms)
        if (substr($name, -5) == '.phpt' && substr($name, 0, 1) !== '.') {
            $testfile = realpath("{$dir}/{$name}");
            if ($ignore) {
                $ignored_by_ext[] = $testfile;
            } else {
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
    }

    return $name;
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
    }

    return $tb - $ta;
}

//
//  Write the given text to a temporary file, and return the filename.
//

function save_text(string $filename, string $text, ?string $filename_copy = null): void
{
    global $DETAILED;

    if ($filename_copy && $filename_copy != $filename && file_put_contents($filename_copy, $text) === false) {
        error("Cannot open file '" . $filename_copy . "' (save_text)");
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

    // when proc_open cmd is passed as a string (without bypass_shell=true option) the cmd goes thru shell
    // and on Windows quotes are discarded, this is a fix to honor the quotes and allow values containing
    // spaces like '"C:\Program Files\PHP\php.exe"' to be passed as 1 argument correctly
    if (IS_WINDOWS) {
        $commandline = 'start "" /b /wait ' . $commandline;
    }

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
    /* ASAN can cause a ~2-3x slowdown. */
    if (isset($env['SKIP_ASAN'])) {
        $timeout *= 3;
    }

    while (true) {
        /* hide errors from interrupted syscalls */
        $r = $pipes;
        $w = null;
        $e = null;

        $n = @stream_select($r, $w, $e, $timeout);

        if ($n === false) {
            break;
        }

        if ($n === 0) {
            /* timed out */
            $data .= "\n ** ERROR: process timed out **\n";
            proc_terminate($proc, 9);
            return $data;
        }

        if ($n > 0) {
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

function run_all_tests(array $test_files, array $env, ?string $redir_tested = null): void
{
    global $test_results, $failed_tests_file, $result_tests_file, $php, $test_idx, $file_cache;
    global $preload;
    // Parallel testing
    global $PHP_FAILED_TESTS, $workers, $workerID, $workerSock;

    if ($file_cache !== null || $preload) {
        /* Automatically skip opcache tests in --file-cache and --preload mode,
         * because opcache generally expects these to run under a default configuration. */
        $test_files = array_filter($test_files, function($test) use($preload) {
            if (!is_string($test)) {
                return true;
            }
            if (false !== strpos($test, 'ext/opcache')) {
                return false;
            }
            if ($preload && false !== strpos($test, 'ext/zend_test/tests/observer')) {
                return false;
            }
            return true;
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

function run_all_tests_parallel(array $test_files, array $env, ?string $redir_tested): void
{
    global $workers, $test_idx, $test_results, $failed_tests_file, $result_tests_file, $PHP_FAILED_TESTS, $shuffle, $valgrind, $show_progress;

    global $junit;

    // The PHP binary running run-tests.php, and run-tests.php itself
    // This PHP executable is *not* necessarily the same as the tested version
    $thisPHP = PHP_BINARY;
    $thisScript = __FILE__;

    $workerProcs = [];
    $workerSocks = [];

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
            [$thisPHP, $thisScript],
            [], // Inherit our stdin, stdout and stderr
            $pipes,
            null,
            $GLOBALS['environment'] + [
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
                if (feof($workerSock)) {
                    kill_children($workerProcs);
                    error("Worker $i died unexpectedly");
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
                            $junit->mergeResults($message["junit"]);
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
                                unset($workerProcs[$i], $workerSocks[$i]);
                                goto escape;
                            }
                            break;
                        case "test_result":
                            list($name, $index, $result, $resultText) = [$message["name"], $message["index"], $message["result"], $message["text"]];
                            foreach ($message["PHP_FAILED_TESTS"] as $category => $tests) {
                                $PHP_FAILED_TESTS[$category] = array_merge($PHP_FAILED_TESTS[$category], $tests);
                            }
                            $test_idx++;

                            if ($show_progress) {
                                clear_show_test();
                            }

                            echo $resultText;

                            if ($show_progress) {
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

    if ($show_progress) {
        clear_show_test();
    }

    kill_children($workerProcs);

    if ($testsInProgress < 0) {
        error("$testsInProgress test batches “in progress”, which is less than zero. THIS SHOULD NOT HAPPEN.");
    }
}

/**
 * Calls fwrite and retries when network writes fail with errors such as "Resource temporarily unavailable"
 *
 * @param resource $stream the stream to fwrite to
 * @param string $data
 * @return int|false
 */
function safe_fwrite($stream, string $data)
{
    // safe_fwrite was tested by adding $message['unused'] = str_repeat('a', 20_000_000); in send_message()
    // fwrites on tcp sockets can return false or less than strlen if the recipient is busy.
    // (e.g. fwrite(): Send of 577 bytes failed with errno=35 Resource temporarily unavailable)
    $bytes_written = 0;
    while ($bytes_written < strlen($data)) {
        $n = @fwrite($stream, substr($data, $bytes_written));
        if ($n === false) {
            $write_streams = [$stream];
            $read_streams = [];
            $except_streams = [];
            /* Wait for up to 10 seconds for the stream to be ready to write again. */
            $result = stream_select($read_streams, $write_streams, $except_streams, 10);
            if (!$result) {
                echo "ERROR: send_message() stream_select() failed\n";
                return false;
            }
            $n = @fwrite($stream, substr($data, $bytes_written));
            if ($n === false) {
                echo "ERROR: send_message() Failed to write chunk after stream_select: " . error_get_last()['message'] . "\n";
                return false;
            }
        }
        $bytes_written += $n;
    }
    return $bytes_written;
}

function send_message($stream, array $message): void
{
    $blocking = stream_get_meta_data($stream)["blocked"];
    stream_set_blocking($stream, true);
    safe_fwrite($stream, base64_encode(serialize($message)) . "\n");
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

    global $junit;

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
                    "junit" => $junit->isEnabled() ? $junit : null,
                ]);
                $junit->clear();
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

function skip_test(string $tested, string $tested_file, string $shortname, string $reason): string
{
    global $junit;

    show_result('SKIP', $tested, $tested_file, "reason: $reason");
    $junit->initSuite($junit->getSuiteName($shortname));
    $junit->markTestAs('SKIP', $shortname, $tested, 0, $reason);
    return 'SKIPPED';
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
    global $num_repeats;
    // Parallel testing
    global $workerID;
    global $show_progress;

    // Temporary
    /** @var JUnit $junit */
    global $junit;

    static $skipCache;
    if (!$skipCache) {
        $enableSkipCache = !($env['DISABLE_SKIP_CACHE'] ?? '0');
        $skipCache = new SkipCache($enableSkipCache, $cfg['keep']['skip']);
    }

    $orig_php = $php;
    $php = escapeshellarg($php);

    $retried = false;
retry:

    $org_file = $file;

    $php_cgi = $env['TEST_PHP_CGI_EXECUTABLE'] ?? null;
    $phpdbg = $env['TEST_PHPDBG_EXECUTABLE'] ?? null;

    if (is_array($file)) {
        $file = $file[0];
    }

    if ($DETAILED) {
        echo "
=================
TEST $file
";
    }

    $shortname = str_replace(TEST_PHP_SRCDIR . '/', '', $file);
    $tested_file = $shortname;

    try {
        $test = new TestFile($file, (bool)$IN_REDIRECT);
    } catch (BorkageException $ex) {
        show_result("BORK", $ex->getMessage(), $tested_file);
        $PHP_FAILED_TESTS['BORKED'][] = [
            'name' => $file,
            'test_name' => '',
            'output' => '',
            'diff' => '',
            'info' => "{$ex->getMessage()} [$file]",
        ];

        $junit->markTestAs('BORK', $shortname, $tested_file, 0, $ex->getMessage());
        return 'BORKED';
    }

    $tested = $test->getName();

    if ($test->hasSection('FILE_EXTERNAL')) {
        if ($num_repeats > 1) {
            return skip_test($tested, $tested_file, $shortname, 'Test with FILE_EXTERNAL might not be repeatable');
        }
    }

    if ($test->hasSection('CAPTURE_STDIO')) {
        $capture = $test->getSection('CAPTURE_STDIO');
        $captureStdIn = stripos($capture, 'STDIN') !== false;
        $captureStdOut = stripos($capture, 'STDOUT') !== false;
        $captureStdErr = stripos($capture, 'STDERR') !== false;
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

    /* For GET/POST/PUT tests, check if cgi sapi is available and if it is, use it. */
    $uses_cgi = false;
    if ($test->isCGI()) {
        if (!$php_cgi) {
            return skip_test($tested, $tested_file, $shortname, 'CGI not available');
        }
        $php = escapeshellarg($php_cgi) . ' -C ';
        $uses_cgi = true;
        if ($num_repeats > 1) {
            return skip_test($tested, $tested_file, $shortname, 'CGI does not support --repeat');
        }
    }

    /* For phpdbg tests, check if phpdbg sapi is available and if it is, use it. */
    $extra_options = '';
    if ($test->hasSection('PHPDBG')) {
        if (isset($phpdbg)) {
            $php = escapeshellarg($phpdbg) . ' -qIb';

            // Additional phpdbg command line options for sections that need to
            // be run straight away. For example, EXTENSIONS, SKIPIF, CLEAN.
            $extra_options = '-rr';
        } else {
            return skip_test($tested, $tested_file, $shortname, 'phpdbg not available');
        }
        if ($num_repeats > 1) {
            return skip_test($tested, $tested_file, $shortname, 'phpdbg does not support --repeat');
        }
    }

    foreach (['CLEAN', 'STDIN', 'CAPTURE_STDIO'] as $section) {
        if ($test->hasSection($section)) {
            if ($num_repeats > 1) {
                return skip_test($tested, $tested_file, $shortname, "Test with $section might not be repeatable");
            }
        }
    }

    if ($show_progress && !$workerID) {
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
        $copy_file = $temp_dir . DIRECTORY_SEPARATOR . basename($file) . '.phps';

        if (!is_dir(dirname($copy_file))) {
            mkdir(dirname($copy_file), 0777, true) or error("Cannot create output directory - " . dirname($copy_file));
        }

        if ($test->hasSection('FILE')) {
            save_text($copy_file, $test->getSection('FILE'));
        }
    }

    if (is_array($IN_REDIRECT)) {
        $tested = $IN_REDIRECT['prefix'] . ' ' . $tested;
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

    if ($test->sectionNotEmpty('ENV')) {
        $env_str = str_replace('{PWD}', dirname($file), $test->getSection('ENV'));
        foreach (explode("\n", $env_str) as $e) {
            $e = explode('=', trim($e), 2);

            if (!empty($e[0]) && isset($e[1])) {
                $env[$e[0]] = $e[1];
            }
        }
    }

    // Default ini settings
    $ini_settings = $workerID ? ['opcache.cache_id' => "worker$workerID"] : [];

    // Additional required extensions
    $extensions = [];
    if ($test->hasSection('EXTENSIONS')) {
        $extensions = preg_split("/[\n\r]+/", trim($test->getSection('EXTENSIONS')));
    }
    if (is_array($IN_REDIRECT) && $IN_REDIRECT['EXTENSIONS'] != []) {
        $extensions = array_merge($extensions, $IN_REDIRECT['EXTENSIONS']);
    }

    /* Load required extensions */
    if ($extensions != []) {
        $ext_params = [];
        settings2array($ini_overwrites, $ext_params);
        $ext_params = settings2params($ext_params);
        [$ext_dir, $loaded] = $skipCache->getExtensions("$orig_php $pass_options $extra_options $ext_params $no_file_cache");
        $ext_prefix = IS_WINDOWS ? "php_" : "";
        $missing = [];
        foreach ($extensions as $req_ext) {
            if (!in_array($req_ext, $loaded, true)) {
                if ($req_ext == 'opcache' || $req_ext == 'xdebug') {
                    $ext_file = $ext_dir . DIRECTORY_SEPARATOR . $ext_prefix . $req_ext . '.' . PHP_SHLIB_SUFFIX;
                    $ini_settings['zend_extension'][] = $ext_file;
                } else {
                    $ext_file = $ext_dir . DIRECTORY_SEPARATOR . $ext_prefix . $req_ext . '.' . PHP_SHLIB_SUFFIX;
                    $ini_settings['extension'][] = $ext_file;
                }
                if (!is_readable($ext_file)) {
                    $missing[] = $req_ext;
                }
            }
        }
        if ($missing) {
            $message = 'Required extension' . (count($missing) > 1 ? 's' : '')
                . ' missing: ' . implode(', ', $missing);
            return skip_test($tested, $tested_file, $shortname, $message);
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
    } else if ($num_repeats > 1) {
        // Make sure warnings still show up on the second run.
        $ini_settings['opcache.record_warnings'] = '1';
    }

    // Any special ini settings
    // these may overwrite the test defaults...
    if ($test->hasSection('INI')) {
        $ini = str_replace('{PWD}', dirname($file), $test->getSection('INI'));
        $ini = str_replace('{TMP}', sys_get_temp_dir(), $ini);
        $replacement = IS_WINDOWS ? '"' . PHP_BINARY . ' -r \"while ($in = fgets(STDIN)) echo $in;\" > $1"' : 'tee $1 >/dev/null';
        $ini = preg_replace('/{MAIL:(\S+)}/', $replacement, $ini);
        $skip = false;
        $ini = preg_replace_callback('/{ENV:(\S+)}/', function ($m) use (&$skip) {
            $name = $m[1];
            $value = getenv($name);
            if ($value === false) {
                $skip = sprintf('Environment variable %s is not set', $name);
                return '';
            }
            return $value;
        }, $ini);
        if ($skip !== false) {
            return skip_test($tested, $tested_file, $shortname, $skip);
        }
        settings2array(preg_split("/[\n\r]+/", $ini), $ini_settings);

        if (isset($ini_settings['opcache.opt_debug_level'])) {
            if ($num_repeats > 1) {
                return skip_test($tested, $tested_file, $shortname, 'opt_debug_level tests are not repeatable');
            }
        }
    }

    $ini_settings = settings2params($ini_settings);

    $env['TEST_PHP_EXTRA_ARGS'] = $pass_options . ' ' . $ini_settings;

    // Check if test should be skipped.
    $info = '';
    $warn = false;

    if ($test->sectionNotEmpty('SKIPIF')) {
        show_file_block('skip', $test->getSection('SKIPIF'));
        $extra = !IS_WINDOWS ?
            "unset REQUEST_METHOD; unset QUERY_STRING; unset PATH_TRANSLATED; unset SCRIPT_FILENAME; unset REQUEST_METHOD;" : "";

        if ($valgrind) {
            $env['USE_ZEND_ALLOC'] = '0';
            $env['ZEND_DONT_UNLOAD_MODULES'] = 1;
        }

        $junit->startTimer($shortname);

        $startTime = microtime(true);
        $commandLine = "$extra $php $pass_options $extra_options -q $orig_ini_settings $no_file_cache -d display_errors=1 -d display_startup_errors=0";
        $output = $skipCache->checkSkip($commandLine, $test->getSection('SKIPIF'), $test_skipif, $temp_skipif, $env);

        $time = microtime(true) - $startTime;
        $junit->stopTimer($shortname);

        if ($time > $slow_min_ms / 1000) {
            $PHP_FAILED_TESTS['SLOW'][] = [
                'name' => $file,
                'test_name' => 'SKIPIF of ' . $tested . " [$tested_file]",
                'output' => '',
                'diff' => '',
                'info' => $time,
            ];
        }

        if (!$cfg['keep']['skip']) {
            @unlink($test_skipif);
        }

        if (!strncasecmp('skip', $output, 4)) {
            if (preg_match('/^skip\s*(.+)/i', $output, $m)) {
                show_result('SKIP', $tested, $tested_file, "reason: $m[1]");
            } else {
                show_result('SKIP', $tested, $tested_file, '');
            }

            $message = !empty($m[1]) ? $m[1] : '';
            $junit->markTestAs('SKIP', $shortname, $tested, null, $message);
            return 'SKIPPED';
        }

        if (!strncasecmp('info', $output, 4) && preg_match('/^info\s*(.+)/i', $output, $m)) {
            $info = " (info: $m[1])";
        } elseif (!strncasecmp('warn', $output, 4) && preg_match('/^warn\s+(.+)/i', $output, $m)) {
            $warn = true; /* only if there is a reason */
            $info = " (warn: $m[1])";
        } elseif (!strncasecmp('xfail', $output, 5)) {
            // Pretend we have an XFAIL section
            $test->setSection('XFAIL', ltrim(substr($output, 5)));
        } elseif (!strncasecmp('xleak', $output, 5)) {
            // Pretend we have an XLEAK section
            $test->setSection('XLEAK', ltrim(substr($output, 5)));
        } elseif ($output !== '') {
            show_result("BORK", $output, $tested_file, 'reason: invalid output from SKIPIF');
            $PHP_FAILED_TESTS['BORKED'][] = [
                'name' => $file,
                'test_name' => '',
                'output' => '',
                'diff' => '',
                'info' => "$output [$file]",
            ];

            $junit->markTestAs('BORK', $shortname, $tested, null, $output);
            return 'BORKED';
        }
    }

    if (!extension_loaded("zlib") && $test->hasAnySections("GZIP_POST", "DEFLATE_POST")) {
        $message = "ext/zlib required";
        show_result('SKIP', $tested, $tested_file, "reason: $message");
        $junit->markTestAs('SKIP', $shortname, $tested, null, $message);
        return 'SKIPPED';
    }

    if ($test->hasSection('REDIRECTTEST')) {
        $test_files = [];

        $IN_REDIRECT = eval($test->getSection('REDIRECTTEST'));
        $IN_REDIRECT['via'] = "via [$shortname]\n\t";
        $IN_REDIRECT['dir'] = realpath(dirname($file));
        $IN_REDIRECT['prefix'] = $tested;
        $IN_REDIRECT['EXTENSIONS'] = $extensions;

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

            $junit->markTestAs('PASS', $shortname, $tested);
            return 'REDIR';
        }

        $bork_info = "Redirect info must contain exactly one TEST string to be used as redirect directory.";
        show_result("BORK", $bork_info, '', '');
        $PHP_FAILED_TESTS['BORKED'][] = [
            'name' => $file,
            'test_name' => '',
            'output' => '',
            'diff' => '',
            'info' => "$bork_info [$file]",
        ];
    }

    if (is_array($org_file) || $test->hasSection('REDIRECTTEST')) {
        if (is_array($org_file)) {
            $file = $org_file[0];
        }

        $bork_info = "Redirected test did not contain redirection info";
        show_result("BORK", $bork_info, '', '');
        $PHP_FAILED_TESTS['BORKED'][] = [
            'name' => $file,
            'test_name' => '',
            'output' => '',
            'diff' => '',
            'info' => "$bork_info [$file]",
        ];

        $junit->markTestAs('BORK', $shortname, $tested, null, $bork_info);

        return 'BORKED';
    }

    // We've satisfied the preconditions - run the test!
    if ($test->hasSection('FILE')) {
        show_file_block('php', $test->getSection('FILE'), 'TEST');
        save_text($test_file, $test->getSection('FILE'), $temp_file);
    } else {
        $test_file = "";
    }

    if ($test->hasSection('GET')) {
        $query_string = trim($test->getSection('GET'));
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

    if ($test->hasSection('COOKIE')) {
        $env['HTTP_COOKIE'] = trim($test->getSection('COOKIE'));
    } else {
        $env['HTTP_COOKIE'] = '';
    }

    $args = $test->hasSection('ARGS') ? ' -- ' . $test->getSection('ARGS') : '';

    if ($preload && !empty($test_file)) {
        save_text($preload_filename, "<?php opcache_compile_file('$test_file');");
        $local_pass_options = $pass_options;
        unset($pass_options);
        $pass_options = $local_pass_options;
        $pass_options .= " -d opcache.preload=" . $preload_filename;
    }

    if ($test->sectionNotEmpty('POST_RAW')) {
        $post = trim($test->getSection('POST_RAW'));
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
        if (empty($env['REQUEST_METHOD'])) {
            $env['REQUEST_METHOD'] = 'POST';
        }

        if (empty($request)) {
            $junit->markTestAs('BORK', $shortname, $tested, null, 'empty $request');
            return 'BORKED';
        }

        save_text($tmp_post, $request);
        $cmd = "$php $pass_options $ini_settings -f \"$test_file\"$cmdRedirect < \"$tmp_post\"";
    } elseif ($test->sectionNotEmpty('PUT')) {
        $post = trim($test->getSection('PUT'));
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
            $junit->markTestAs('BORK', $shortname, $tested, null, 'empty $request');
            return 'BORKED';
        }

        save_text($tmp_post, $request);
        $cmd = "$php $pass_options $ini_settings -f \"$test_file\"$cmdRedirect < \"$tmp_post\"";
    } elseif ($test->sectionNotEmpty('POST')) {
        $post = trim($test->getSection('POST'));
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
    } elseif ($test->sectionNotEmpty('GZIP_POST')) {
        $post = trim($test->getSection('GZIP_POST'));
        $post = gzencode($post, 9, FORCE_GZIP);
        $env['HTTP_CONTENT_ENCODING'] = 'gzip';

        save_text($tmp_post, $post);
        $content_length = strlen($post);

        $env['REQUEST_METHOD'] = 'POST';
        $env['CONTENT_TYPE'] = 'application/x-www-form-urlencoded';
        $env['CONTENT_LENGTH'] = $content_length;

        $cmd = "$php $pass_options $ini_settings -f \"$test_file\"$cmdRedirect < \"$tmp_post\"";
    } elseif ($test->sectionNotEmpty('DEFLATE_POST')) {
        $post = trim($test->getSection('DEFLATE_POST'));
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

        $repeat_option = $num_repeats > 1 ? "--repeat $num_repeats" : "";
        $cmd = "$php $pass_options $repeat_option $ini_settings -f \"$test_file\" $args$cmdRedirect";
    }

    $orig_cmd = $cmd;
    if ($valgrind) {
        $env['USE_ZEND_ALLOC'] = '0';
        $env['ZEND_DONT_UNLOAD_MODULES'] = 1;

        $cmd = $valgrind->wrapCommand($cmd, $memcheck_filename, strpos($test_file, "pcre") !== false);
    }

    if ($test->hasSection('XLEAK')) {
        $env['ZEND_ALLOC_PRINT_LEAKS'] = '0';
        if (isset($env['SKIP_ASAN'])) {
            // $env['LSAN_OPTIONS'] = 'detect_leaks=0';
            /* For unknown reasons, LSAN_OPTIONS=detect_leaks=0 would occasionally not be picked up
             * in CI. Skip the test with ASAN, as it's not worth investegating. */
            return skip_test($tested, $tested_file, $shortname, 'xleak does not work with asan');
        }
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

    $junit->startTimer($shortname);
    $hrtime = hrtime();
    $startTime = $hrtime[0] * 1000000000 + $hrtime[1];

    $stdin = $test->hasSection('STDIN') ? $test->getSection('STDIN') : null;
    $out = system_with_timeout($cmd, $env, $stdin, $captureStdIn, $captureStdOut, $captureStdErr);

    $junit->stopTimer($shortname);
    $hrtime = hrtime();
    $time = $hrtime[0] * 1000000000 + $hrtime[1] - $startTime;
    if ($time >= $slow_min_ms * 1000000) {
        $PHP_FAILED_TESTS['SLOW'][] = [
            'name' => $file,
            'test_name' => $tested . " [$tested_file]",
            'output' => '',
            'diff' => '',
            'info' => $time / 1000000000,
        ];
    }

    // Remember CLEAN output to report borked test if it otherwise passes.
    $clean_output = null;
    if ((!$no_clean || $cfg['keep']['clean']) && $test->sectionNotEmpty('CLEAN')) {
        show_file_block('clean', $test->getSection('CLEAN'));
        save_text($test_clean, trim($test->getSection('CLEAN')), $temp_clean);

        if (!$no_clean) {
            $extra = !IS_WINDOWS ?
                "unset REQUEST_METHOD; unset QUERY_STRING; unset PATH_TRANSLATED; unset SCRIPT_FILENAME; unset REQUEST_METHOD;" : "";
            $clean_output = system_with_timeout("$extra $orig_php $pass_options -q $orig_ini_settings $no_file_cache \"$test_clean\"", $env);
        }

        if (!$cfg['keep']['clean']) {
            @unlink($test_clean);
        }
    }

    $leaked = false;
    $passed = false;

    if ($valgrind) { // leak check
        $leaked = filesize($memcheck_filename) > 0;

        if (!$leaked) {
            @unlink($memcheck_filename);
        }
    }

    if ($num_repeats > 1) {
        // In repeat mode, retain the output before the first execution,
        // and of the last execution. Do this early, because the trimming below
        // makes the newline handling complicated.
        $separator1 = "Executing for the first time...\n";
        $separator1_pos = strpos($out, $separator1);
        if ($separator1_pos !== false) {
            $separator2 = "Finished execution, repeating...\n";
            $separator2_pos = strrpos($out, $separator2);
            if ($separator2_pos !== false) {
                $out = substr($out, 0, $separator1_pos)
                     . substr($out, $separator2_pos + strlen($separator2));
            } else {
                $out = substr($out, 0, $separator1_pos)
                     . substr($out, $separator1_pos + strlen($separator1));
            }
        }
    }

    // Does the output match what is expected?
    $output = preg_replace("/\r\n/", "\n", trim($out));

    /* when using CGI, strip the headers from the output */
    $headers = [];

    if ($uses_cgi && preg_match("/^(.*?)\r?\n\r?\n(.*)/s", $out, $match)) {
        $output = trim($match[2]);
        $rh = preg_split("/[\n\r]+/", $match[1]);

        foreach ($rh as $line) {
            if (strpos($line, ':') !== false) {
                $line = explode(':', $line, 2);
                $headers[trim($line[0])] = trim($line[1]);
            }
        }
    }

    $wanted_headers = null;
    $output_headers = null;
    $failed_headers = false;

    if ($test->hasSection('EXPECTHEADERS')) {
        $want = [];
        $wanted_headers = [];
        $lines = preg_split("/[\n\r]+/", $test->getSection('EXPECTHEADERS'));

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

        $wanted_headers = implode("\n", $wanted_headers);
        $output_headers = implode("\n", $output_headers);
    }

    show_file_block('out', $output);

    if ($preload) {
        $output = trim(preg_replace("/\n?Warning: Can't preload [^\n]*\n?/", "", $output));
    }

    if ($test->hasAnySections('EXPECTF', 'EXPECTREGEX')) {
        if ($test->hasSection('EXPECTF')) {
            $wanted = trim($test->getSection('EXPECTF'));
        } else {
            $wanted = trim($test->getSection('EXPECTREGEX'));
        }

        show_file_block('exp', $wanted);
        $wanted_re = preg_replace('/\r\n/', "\n", $wanted);

        if ($test->hasSection('EXPECTF')) {
            $wanted_re = expectf_to_regex($wanted_re);
        }

        if (preg_match('/^' . $wanted_re . '$/s', $output)) {
            $passed = true;
        }
    } else {
        $wanted = trim($test->getSection('EXPECT'));
        $wanted = preg_replace('/\r\n/', "\n", $wanted);
        show_file_block('exp', $wanted);

        // compare and leave on success
        if (!strcmp($output, $wanted)) {
            $passed = true;
        }

        $wanted_re = null;
    }
    if (!$passed && !$retried && error_may_be_retried($test, $output)) {
        $retried = true;
        goto retry;
    }

    if ($passed) {
        if (!$cfg['keep']['php'] && !$leaked) {
            @unlink($test_file);
            @unlink($preload_filename);
        }
        @unlink($tmp_post);

        if (!$leaked && !$failed_headers) {
            // If the test passed and CLEAN produced output, report test as borked.
            if ($clean_output) {
                show_result("BORK", $output, $tested_file, 'reason: invalid output from CLEAN');
                $PHP_FAILED_TESTS['BORKED'][] = [
                    'name' => $file,
                    'test_name' => '',
                    'output' => '',
                    'diff' => '',
                    'info' => "$clean_output [$file]",
                ];

                $junit->markTestAs('BORK', $shortname, $tested, null, $clean_output);
                return 'BORKED';
            }

            if ($test->hasSection('XFAIL')) {
                $warn = true;
                $info = " (warn: XFAIL section but test passes)";
            } elseif ($test->hasSection('XLEAK') && $valgrind) {
                // XLEAK with ASAN completely disables LSAN so the test is expected to pass
                $warn = true;
                $info = " (warn: XLEAK section but test passes)";
            } elseif ($retried) {
                $warn = true;
                $info = " (warn: Test passed on retry attempt)";
            } else {
                show_result("PASS", $tested, $tested_file, '');
                $junit->markTestAs('PASS', $shortname, $tested);
                return 'PASSED';
            }
        }
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

    $restype = [];

    if ($leaked) {
        $restype[] = $test->hasSection('XLEAK') ?
                        'XLEAK' : 'LEAK';
    }

    if ($warn) {
        $restype[] = 'WARN';
    }

    if (!$passed) {
        if ($test->hasSection('XFAIL')) {
            $restype[] = 'XFAIL';
            $info = '  XFAIL REASON: ' . rtrim($test->getSection('XFAIL'));
        } elseif ($test->hasSection('XLEAK') && $valgrind) {
            // XLEAK with ASAN completely disables LSAN so the test is expected to pass
            $restype[] = 'XLEAK';
            $info = '  XLEAK REASON: ' . rtrim($test->getSection('XLEAK'));
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
        if (!empty($environment['TEST_PHP_DIFF_CMD'])) {
            $diff = generate_diff_external($environment['TEST_PHP_DIFF_CMD'], $exp_filename, $output_filename);
        } else {
            $diff = generate_diff($wanted, $wanted_re, $output);
        }

        if (is_array($IN_REDIRECT)) {
            $orig_shortname = str_replace(TEST_PHP_SRCDIR . '/', '', $file);
            $diff = "# original source file: $orig_shortname\n" . $diff;
        }
        if (!$SHOW_ONLY_GROUPS || array_intersect($restype, $SHOW_ONLY_GROUPS)) {
            show_file_block('diff', $diff);
        }
        if (strpos($log_format, 'D') !== false && file_put_contents($diff_filename, $diff) === false) {
            error("Cannot create test diff - $diff_filename");
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

    if (!$passed || $leaked) {
        // write .sh
        if (strpos($log_format, 'S') !== false) {
            $env_lines = [];
            foreach ($env as $env_var => $env_val) {
                $env_lines[] = "export $env_var=" . escapeshellarg($env_val ?? "");
            }
            $exported_environment = "\n" . implode("\n", $env_lines) . "\n";
            $sh_script = <<<SH
#!/bin/sh
{$exported_environment}
case "$1" in
"gdb")
    gdb --args {$orig_cmd}
    ;;
"lldb")
    lldb -- {$orig_cmd}
    ;;
"valgrind")
    USE_ZEND_ALLOC=0 valgrind $2 {$orig_cmd}
    ;;
"rr")
    rr record $2 {$orig_cmd}
    ;;
*)
    {$orig_cmd}
    ;;
esac
SH;
            if (file_put_contents($sh_filename, $sh_script) === false) {
                error("Cannot create test shell script - $sh_filename");
            }
            chmod($sh_filename, 0755);
        }
    }

    if ($valgrind && $leaked && $cfg["show"]["mem"]) {
        show_file_block('mem', file_get_contents($memcheck_filename));
    }

    show_result(implode('&', $restype), $tested, $tested_file, $info);

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

    $junit->markTestAs($restype, $shortname, $tested, null, $info, $diff);

    return $restype[0] . 'ED';
}

function is_flaky(TestFile $test): bool
{
    if ($test->hasSection('FLAKY')) {
        return true;
    }
    if (!$test->hasSection('FILE')) {
        return false;
    }
    $file = $test->getSection('FILE');
    $flaky_functions = [
        'disk_free_space',
        'hrtime',
        'microtime',
        'sleep',
        'usleep',
    ];
    $regex = '(\b(' . implode('|', $flaky_functions) . ')\()i';
    return preg_match($regex, $file) === 1;
}

function is_flaky_output(string $output): bool
{
    $messages = [
        '404: page not found',
        'address already in use',
        'connection refused',
        'deadlock',
        'mailbox already exists',
        'timed out',
    ];
    $regex = '(\b(' . implode('|', $messages) . ')\b)i';
    return preg_match($regex, $output) === 1;
}

function error_may_be_retried(TestFile $test, string $output): bool
{
    return is_flaky_output($output)
        || is_flaky($test);
}

function expectf_to_regex(?string $wanted): string
{
    $wanted_re = $wanted ?? '';

    $wanted_re = preg_replace('/\r\n/', "\n", $wanted_re);

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

    return strtr($wanted_re, [
        '%e' => preg_quote(DIRECTORY_SEPARATOR, '/'),
        '%s' => '[^\r\n]+',
        '%S' => '[^\r\n]*',
        '%a' => '.+',
        '%A' => '.*',
        '%w' => '\s*',
        '%i' => '[+-]?\d+',
        '%d' => '\d+',
        '%x' => '[0-9a-fA-F]+',
        '%f' => '[+-]?(?:\d+|(?=\.\d))(?:\.\d+)?(?:[Ee][+-]?\d+)?',
        '%c' => '.',
        '%0' => '\x00',
    ]);
}

/**
 * @return bool|int
 */
function comp_line(string $l1, string $l2, bool $is_reg)
{
    if ($is_reg) {
        return preg_match('/^' . $l1 . '$/s', $l2);
    }

    return !strcmp($l1, $l2);
}

/**
 * Map "Zend OPcache" to "opcache" and convert all ext names to lowercase.
 */
function remap_loaded_extensions_names(array $names): array
{
    $exts = [];
    foreach ($names as $name) {
        if ($name === 'Core') {
            continue;
        }
        $exts[] = ['Zend OPcache' => 'opcache'][$name] ?? strtolower($name);
    }

    return $exts;
}

function generate_diff_external(string $diff_cmd, string $exp_file, string $output_file): string
{
    $retval = shell_exec("{$diff_cmd} {$exp_file} {$output_file}");

    return is_string($retval) ? $retval : 'Could not run external diff tool set through TEST_PHP_DIFF_CMD environment variable';
}

function generate_diff(string $wanted, ?string $wanted_re, string $output): string
{
    $w = explode("\n", $wanted);
    $o = explode("\n", $output);
    $is_regex = $wanted_re !== null;

    $differ = new Differ(function ($expected, $new) use ($is_regex) {
        if (!$is_regex) {
            return $expected === $new;
        }
        $regex = '/^' . expectf_to_regex($expected). '$/s';
        return preg_match($regex, $new);
    });
    return $differ->diff($w, $o);
}

function error(string $message): void
{
    echo "ERROR: {$message}\n";
    exit(1);
}

function settings2array(array $settings, array &$ini_settings): void
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
    $n_total += count($ignored_by_ext);
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

    $sum_results['SKIPPED'] += count($ignored_by_ext);
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
Exts skipped    : ' . sprintf('%5d', count($exts_skipped)) . ($exts_skipped ? ' (' . implode(', ', $exts_skipped) . ')' : '') . '
Exts tested     : ' . sprintf('%5d', count($exts_tested)) . '
---------------------------------------------------------------------
';
    }

    $summary .= '
Number of tests : ' . sprintf('%5d', $n_total) . '          ' . sprintf('%8d', $x_total);

    if ($sum_results['BORKED']) {
        $summary .= '
Tests borked    : ' . sprintf('%5d (%5.1f%%)', $sum_results['BORKED'], $percent_results['BORKED']) . ' --------';
    }

    $summary .= '
Tests skipped   : ' . sprintf('%5d (%5.1f%%)', $sum_results['SKIPPED'], $percent_results['SKIPPED']) . ' --------
Tests warned    : ' . sprintf('%5d (%5.1f%%)', $sum_results['WARNED'], $percent_results['WARNED']) . ' ' . sprintf('(%5.1f%%)', $x_warned) . '
Tests failed    : ' . sprintf('%5d (%5.1f%%)', $sum_results['FAILED'], $percent_results['FAILED']) . ' ' . sprintf('(%5.1f%%)', $x_failed);

    if ($sum_results['XFAILED']) {
        $summary .= '
Expected fail   : ' . sprintf('%5d (%5.1f%%)', $sum_results['XFAILED'], $percent_results['XFAILED']) . ' ' . sprintf('(%5.1f%%)', $x_xfailed);
    }

    if ($valgrind) {
        $summary .= '
Tests leaked    : ' . sprintf('%5d (%5.1f%%)', $sum_results['LEAKED'], $percent_results['LEAKED']) . ' ' . sprintf('(%5.1f%%)', $x_leaked);
        if ($sum_results['XLEAKED']) {
            $summary .= '
Expected leak   : ' . sprintf('%5d (%5.1f%%)', $sum_results['XLEAKED'], $percent_results['XLEAKED']) . ' ' . sprintf('(%5.1f%%)', $x_xleaked);
        }
    }

    $summary .= '
Tests passed    : ' . sprintf('%5d (%5.1f%%)', $sum_results['PASSED'], $percent_results['PASSED']) . ' ' . sprintf('(%5.1f%%)', $x_passed) . '
---------------------------------------------------------------------
Time taken      : ' . sprintf('%5.3f seconds', ($end_time - $start_time) / 1e9) . '
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

function show_start(int $start_timestamp): void
{
    echo "TIME START " . date('Y-m-d H:i:s', $start_timestamp) . "\n=====================================================================\n";
}

function show_end(int $start_timestamp, int|float $start_time, int|float $end_time): void
{
    echo "=====================================================================\nTIME END " . date('Y-m-d H:i:s', $start_timestamp + (int)(($end_time - $start_time)/1e9)) . "\n";
}

function show_summary(): void
{
    echo get_summary(true);
}

function show_redirect_start(string $tests, string $tested, string $tested_file): void
{
    global $SHOW_ONLY_GROUPS, $show_progress;

    if (!$SHOW_ONLY_GROUPS || in_array('REDIRECT', $SHOW_ONLY_GROUPS)) {
        echo "REDIRECT $tests ($tested [$tested_file]) begin\n";
    } elseif ($show_progress) {
        clear_show_test();
    }
}

function show_redirect_ends(string $tests, string $tested, string $tested_file): void
{
    global $SHOW_ONLY_GROUPS, $show_progress;

    if (!$SHOW_ONLY_GROUPS || in_array('REDIRECT', $SHOW_ONLY_GROUPS)) {
        echo "REDIRECT $tests ($tested [$tested_file]) done\n";
    } elseif ($show_progress) {
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

    if (!$workerID && isset($line_length)) {
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
    string $extra = ''
): void {
    global $SHOW_ONLY_GROUPS, $colorize, $show_progress;

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
    } elseif ($show_progress) {
        clear_show_test();
    }
}

class BorkageException extends Exception
{
}

class JUnit
{
    private bool $enabled = true;
    private $fp = null;
    private array $suites = [];
    private array $rootSuite = self::EMPTY_SUITE + ['name' => 'php'];

    private const EMPTY_SUITE = [
        'test_total' => 0,
        'test_pass' => 0,
        'test_fail' => 0,
        'test_error' => 0,
        'test_skip' => 0,
        'test_warn' => 0,
        'files' => [],
        'execution_time' => 0,
    ];

    /**
     * @throws Exception
     */
    public function __construct(array $env, int $workerID)
    {
        // Check whether a junit log is wanted.
        $fileName = $env['TEST_PHP_JUNIT'] ?? null;
        if (empty($fileName)) {
            $this->enabled = false;
            return;
        }
        if (!$workerID && !$this->fp = fopen($fileName, 'w')) {
            throw new Exception("Failed to open $fileName for writing.");
        }
    }

    public function isEnabled(): bool
    {
        return $this->enabled;
    }

    public function clear(): void
    {
        $this->rootSuite = self::EMPTY_SUITE + ['name' => 'php'];
        $this->suites = [];
    }

    public function saveXML(): void
    {
        if (!$this->enabled) {
            return;
        }

        $xml = '<' . '?' . 'xml version="1.0" encoding="UTF-8"' . '?' . '>' . PHP_EOL;
        $xml .= sprintf(
            '<testsuites name="%s" tests="%s" failures="%d" errors="%d" skip="%d" time="%s">' . PHP_EOL,
            $this->rootSuite['name'],
            $this->rootSuite['test_total'],
            $this->rootSuite['test_fail'],
            $this->rootSuite['test_error'],
            $this->rootSuite['test_skip'],
            $this->rootSuite['execution_time']
        );
        $xml .= $this->getSuitesXML();
        $xml .= '</testsuites>';
        fwrite($this->fp, $xml);
    }

    private function getSuitesXML(): string
    {
        $result = '';

        foreach ($this->suites as $suite_name => $suite) {
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
                    $result .= $this->rootSuite['files'][$file]['xml'];
                }
            }

            $result .= '</testsuite>' . PHP_EOL;
        }

        return $result;
    }

    public function markTestAs(
        $type,
        string $file_name,
        string $test_name,
        ?int $time = null,
        string $message = '',
        string $details = ''
    ): void {
        if (!$this->enabled) {
            return;
        }

        $suite = $this->getSuiteName($file_name);

        $this->record($suite, 'test_total');

        $time = $time ?? $this->getTimer($file_name);
        $this->record($suite, 'execution_time', $time);

        $escaped_details = htmlspecialchars($details, ENT_QUOTES, 'UTF-8');
        $escaped_details = preg_replace_callback('/[\0-\x08\x0B\x0C\x0E-\x1F]/', function ($c) {
            return sprintf('[[0x%02x]]', ord($c[0]));
        }, $escaped_details);
        $escaped_message = htmlspecialchars($message, ENT_QUOTES, 'UTF-8');

        $escaped_test_name = htmlspecialchars($file_name . ' (' . $test_name . ')', ENT_QUOTES);
        $this->rootSuite['files'][$file_name]['xml'] = "<testcase name='$escaped_test_name' time='$time'>\n";

        if (is_array($type)) {
            $output_type = $type[0] . 'ED';
            $temp = array_intersect(['XFAIL', 'XLEAK', 'FAIL', 'WARN'], $type);
            $type = reset($temp);
        } else {
            $output_type = $type . 'ED';
        }

        if ('PASS' == $type || 'XFAIL' == $type || 'XLEAK' == $type) {
            $this->record($suite, 'test_pass');
        } elseif ('BORK' == $type) {
            $this->record($suite, 'test_error');
            $this->rootSuite['files'][$file_name]['xml'] .= "<error type='$output_type' message='$escaped_message'/>\n";
        } elseif ('SKIP' == $type) {
            $this->record($suite, 'test_skip');
            $this->rootSuite['files'][$file_name]['xml'] .= "<skipped>$escaped_message</skipped>\n";
        } elseif ('WARN' == $type) {
            $this->record($suite, 'test_warn');
            $this->rootSuite['files'][$file_name]['xml'] .= "<warning>$escaped_message</warning>\n";
        } elseif ('FAIL' == $type) {
            $this->record($suite, 'test_fail');
            $this->rootSuite['files'][$file_name]['xml'] .= "<failure type='$output_type' message='$escaped_message'>$escaped_details</failure>\n";
        } else {
            $this->record($suite, 'test_error');
            $this->rootSuite['files'][$file_name]['xml'] .= "<error type='$output_type' message='$escaped_message'>$escaped_details</error>\n";
        }

        $this->rootSuite['files'][$file_name]['xml'] .= "</testcase>\n";
    }

    private function record(string $suite, string $param, $value = 1): void
    {
        $this->rootSuite[$param] += $value;
        $this->suites[$suite][$param] += $value;
    }

    private function getTimer(string $file_name)
    {
        if (!$this->enabled) {
            return 0;
        }

        if (isset($this->rootSuite['files'][$file_name]['total'])) {
            return number_format($this->rootSuite['files'][$file_name]['total'], 4);
        }

        return 0;
    }

    public function startTimer(string $file_name): void
    {
        if (!$this->enabled) {
            return;
        }

        if (!isset($this->rootSuite['files'][$file_name]['start'])) {
            $this->rootSuite['files'][$file_name]['start'] = microtime(true);

            $suite = $this->getSuiteName($file_name);
            $this->initSuite($suite);
            $this->suites[$suite]['files'][$file_name] = $file_name;
        }
    }

    public function getSuiteName(string $file_name): string
    {
        return $this->pathToClassName(dirname($file_name));
    }

    private function pathToClassName(string $file_name): string
    {
        if (!$this->enabled) {
            return '';
        }

        $ret = $this->rootSuite['name'];
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

        return $this->rootSuite['name'] . '.' . str_replace([DIRECTORY_SEPARATOR, '-'], '.', $file_name);
    }

    public function initSuite(string $suite_name): void
    {
        if (!$this->enabled) {
            return;
        }

        if (!empty($this->suites[$suite_name])) {
            return;
        }

        $this->suites[$suite_name] = self::EMPTY_SUITE + ['name' => $suite_name];
    }

    /**
     * @throws Exception
     */
    public function stopTimer(string $file_name): void
    {
        if (!$this->enabled) {
            return;
        }

        if (!isset($this->rootSuite['files'][$file_name]['start'])) {
            throw new Exception("Timer for $file_name was not started!");
        }

        if (!isset($this->rootSuite['files'][$file_name]['total'])) {
            $this->rootSuite['files'][$file_name]['total'] = 0;
        }

        $start = $this->rootSuite['files'][$file_name]['start'];
        $this->rootSuite['files'][$file_name]['total'] += microtime(true) - $start;
        unset($this->rootSuite['files'][$file_name]['start']);
    }

    public function mergeResults(?JUnit $other): void
    {
        if (!$this->enabled || !$other) {
            return;
        }

        $this->mergeSuites($this->rootSuite, $other->rootSuite);
        foreach ($other->suites as $name => $suite) {
            if (!isset($this->suites[$name])) {
                $this->suites[$name] = $suite;
                continue;
            }

            $this->mergeSuites($this->suites[$name], $suite);
        }
    }

    private function mergeSuites(array &$dest, array $source): void
    {
        $dest['test_total'] += $source['test_total'];
        $dest['test_pass']  += $source['test_pass'];
        $dest['test_fail']  += $source['test_fail'];
        $dest['test_error'] += $source['test_error'];
        $dest['test_skip']  += $source['test_skip'];
        $dest['test_warn']  += $source['test_warn'];
        $dest['execution_time'] += $source['execution_time'];
        $dest['files'] += $source['files'];
    }
}

class SkipCache
{
    private bool $enable;
    private bool $keepFile;

    private array $skips = [];
    private array $extensions = [];

    private int $hits = 0;
    private int $misses = 0;
    private int $extHits = 0;
    private int $extMisses = 0;

    public function __construct(bool $enable, bool $keepFile)
    {
        $this->enable = $enable;
        $this->keepFile = $keepFile;
    }

    public function checkSkip(string $php, string $code, string $checkFile, string $tempFile, array $env): string
    {
        // Extension tests frequently use something like <?php require 'skipif.inc';
        // for skip checks. This forces us to cache per directory to avoid pollution.
        $dir = dirname($checkFile);
        $key = "$php => $dir";

        if (isset($this->skips[$key][$code])) {
            $this->hits++;
            if ($this->keepFile) {
                save_text($checkFile, $code, $tempFile);
            }
            return $this->skips[$key][$code];
        }

        save_text($checkFile, $code, $tempFile);
        $result = trim(system_with_timeout("$php \"$checkFile\"", $env));
        if (strpos($result, 'nocache') === 0) {
            $result = '';
        } else if ($this->enable) {
            $this->skips[$key][$code] = $result;
        }
        $this->misses++;

        if (!$this->keepFile) {
            @unlink($checkFile);
        }

        return $result;
    }

    public function getExtensions(string $php): array
    {
        if (isset($this->extensions[$php])) {
            $this->extHits++;
            return $this->extensions[$php];
        }

        $extDir = shell_exec("$php -d display_errors=0 -r \"echo ini_get('extension_dir');\"");
        $extensionsNames = explode(",", shell_exec("$php -d display_errors=0 -r \"echo implode(',', get_loaded_extensions());\""));
        $extensions = remap_loaded_extensions_names($extensionsNames);

        $result = [$extDir, $extensions];
        $this->extensions[$php] = $result;
        $this->extMisses++;

        return $result;
    }
}

class RuntestsValgrind
{
    protected string $header;
    protected bool $version_3_8_0;
    protected string $tool;

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
        $this->header = sprintf("%s (%s)", trim($header), $this->tool);
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
            return "$vcmd --vex-iropt-register-updates=allregs-at-mem-access --log-file=$memcheck_filename $cmd";
        }
        return "$vcmd --vex-iropt-precise-memory-exns=yes --log-file=$memcheck_filename $cmd";
    }
}

class TestFile
{
    private string $fileName;

    private array $sections = ['TEST' => ''];

    private const ALLOWED_SECTIONS = [
        'EXPECT', 'EXPECTF', 'EXPECTREGEX', 'EXPECTREGEX_EXTERNAL', 'EXPECT_EXTERNAL', 'EXPECTF_EXTERNAL', 'EXPECTHEADERS',
        'POST', 'POST_RAW', 'GZIP_POST', 'DEFLATE_POST', 'PUT', 'GET', 'COOKIE', 'ARGS',
        'FILE', 'FILEEOF', 'FILE_EXTERNAL', 'REDIRECTTEST',
        'CAPTURE_STDIO', 'STDIN', 'CGI', 'PHPDBG',
        'INI', 'ENV', 'EXTENSIONS',
        'SKIPIF', 'XFAIL', 'XLEAK', 'CLEAN',
        'CREDITS', 'DESCRIPTION', 'CONFLICTS', 'WHITESPACE_SENSITIVE',
        'FLAKY',
    ];

    /**
     * @throws BorkageException
     */
    public function __construct(string $fileName, bool $inRedirect)
    {
        $this->fileName = $fileName;

        $this->readFile();
        $this->validateAndProcess($inRedirect);
    }

    public function hasSection(string $name): bool
    {
        return isset($this->sections[$name]);
    }

    public function hasAnySections(string ...$names): bool
    {
        foreach ($names as $section) {
            if (isset($this->sections[$section])) {
                return true;
            }
        }

        return false;
    }

    public function sectionNotEmpty(string $name): bool
    {
        return !empty($this->sections[$name]);
    }

    /**
     * @throws Exception
     */
    public function getSection(string $name): string
    {
        if (!isset($this->sections[$name])) {
            throw new Exception("Section $name not found");
        }
        return $this->sections[$name];
    }

    public function getName(): string
    {
        return trim($this->getSection('TEST'));
    }

    public function isCGI(): bool
    {
        return $this->hasSection('CGI')
            || $this->sectionNotEmpty('GET')
            || $this->sectionNotEmpty('POST')
            || $this->sectionNotEmpty('GZIP_POST')
            || $this->sectionNotEmpty('DEFLATE_POST')
            || $this->sectionNotEmpty('POST_RAW')
            || $this->sectionNotEmpty('PUT')
            || $this->sectionNotEmpty('COOKIE')
            || $this->sectionNotEmpty('EXPECTHEADERS');
    }

    /**
     * TODO Refactor to make it not needed
     */
    public function setSection(string $name, string $value): void
    {
        $this->sections[$name] = $value;
    }

    /**
     * Load the sections of the test file
     * @throws BorkageException
     */
    private function readFile(): void
    {
        $fp = fopen($this->fileName, "rb") or error("Cannot open test file: {$this->fileName}");

        if (!feof($fp)) {
            $line = fgets($fp);

            if ($line === false) {
                throw new BorkageException("cannot read test");
            }
        } else {
            throw new BorkageException("empty test [{$this->fileName}]");
        }
        if (strncmp('--TEST--', $line, 8)) {
            throw new BorkageException("tests must start with --TEST-- [{$this->fileName}]");
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
                $section = $r[1];

                if (isset($this->sections[$section]) && $this->sections[$section]) {
                    throw new BorkageException("duplicated $section section");
                }

                // check for unknown sections
                if (!in_array($section, self::ALLOWED_SECTIONS)) {
                    throw new BorkageException('Unknown section "' . $section . '"');
                }

                $this->sections[$section] = '';
                $secfile = $section == 'FILE' || $section == 'FILEEOF' || $section == 'FILE_EXTERNAL';
                $secdone = false;
                continue;
            }

            // Add to the section text.
            if (!$secdone) {
                $this->sections[$section] .= $line;
            }

            // End of actual test?
            if ($secfile && preg_match('/^===DONE===\s*$/', $line)) {
                $secdone = true;
            }
        }

        fclose($fp);
    }

    /**
     * @throws BorkageException
     */
    private function validateAndProcess(bool $inRedirect): void
    {
        // the redirect section allows a set of tests to be reused outside of
        // a given test dir
        if ($this->hasSection('REDIRECTTEST')) {
            if ($inRedirect) {
                throw new BorkageException("Can't redirect a test from within a redirected test");
            }
            return;
        }
        if (!$this->hasSection('PHPDBG') && $this->hasSection('FILE') + $this->hasSection('FILEEOF') + $this->hasSection('FILE_EXTERNAL') != 1) {
            throw new BorkageException("missing section --FILE--");
        }

        if ($this->hasSection('FILEEOF')) {
            $this->sections['FILE'] = preg_replace("/[\r\n]+$/", '', $this->sections['FILEEOF']);
            unset($this->sections['FILEEOF']);
        }

        foreach (['FILE', 'EXPECT', 'EXPECTF', 'EXPECTREGEX'] as $prefix) {
            // For grepping: FILE_EXTERNAL, EXPECT_EXTERNAL, EXPECTF_EXTERNAL, EXPECTREGEX_EXTERNAL
            $key = $prefix . '_EXTERNAL';

            if ($this->hasSection($key)) {
                // don't allow tests to retrieve files from anywhere but this subdirectory
                $dir = dirname($this->fileName);
                $fileName = $dir . '/' . trim(str_replace('..', '', $this->getSection($key)));

                if (file_exists($fileName)) {
                    $this->sections[$prefix] = file_get_contents($fileName);
                } else {
                    throw new BorkageException("could not load --" . $key . "-- " . $dir . '/' . trim($fileName));
                }
            }
        }

        if (($this->hasSection('EXPECT') + $this->hasSection('EXPECTF') + $this->hasSection('EXPECTREGEX')) != 1) {
            throw new BorkageException("missing section --EXPECT--, --EXPECTF-- or --EXPECTREGEX--");
        }

        if ($this->hasSection('PHPDBG') && !$this->hasSection('STDIN')) {
            $this->sections['STDIN'] = $this->sections['PHPDBG'] . "\n";
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

function bless_failed_tests(array $failedTests): void
{
    if (empty($failedTests)) {
        return;
    }
    $args = [
        PHP_BINARY,
        __DIR__ . '/scripts/dev/bless_tests.php',
    ];
    foreach ($failedTests as $test) {
        $args[] = $test['name'];
    }
    proc_open($args, [], $pipes);
}

/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2002-2023, Sebastian Bergmann
 * All rights reserved.
 *
 * This file is part of sebastian/diff.
 * https://github.com/sebastianbergmann/diff
 */

final class Differ
{
    public const OLD = 0;
    public const ADDED = 1;
    public const REMOVED = 2;
    private DiffOutputBuilder $outputBuilder;
    private $isEqual;

    public function __construct(callable $isEqual)
    {
        $this->outputBuilder = new DiffOutputBuilder;
        $this->isEqual = $isEqual;
    }

    public function diff(array $from, array $to): string
    {
        $diff = $this->diffToArray($from, $to);

        return $this->outputBuilder->getDiff($diff);
    }

    public function diffToArray(array $from, array $to): array
    {
        $fromLine = 1;
        $toLine = 1;

        [$from, $to, $start, $end] = $this->getArrayDiffParted($from, $to);

        $common = $this->calculateCommonSubsequence(array_values($from), array_values($to));
        $diff   = [];

        foreach ($start as $token) {
            $diff[] = [$token, self::OLD];
            $fromLine++;
            $toLine++;
        }

        reset($from);
        reset($to);

        foreach ($common as $token) {
            while (!empty($from) && !($this->isEqual)(reset($from), $token)) {
                $diff[] = [array_shift($from), self::REMOVED, $fromLine++];
            }

            while (!empty($to) && !($this->isEqual)($token, reset($to))) {
                $diff[] = [array_shift($to), self::ADDED, $toLine++];
            }

            $diff[] = [$token, self::OLD];
            $fromLine++;
            $toLine++;

            array_shift($from);
            array_shift($to);
        }

        while (($token = array_shift($from)) !== null) {
            $diff[] = [$token, self::REMOVED, $fromLine++];
        }

        while (($token = array_shift($to)) !== null) {
            $diff[] = [$token, self::ADDED, $toLine++];
        }

        foreach ($end as $token) {
            $diff[] = [$token, self::OLD];
        }

        return $diff;
    }

    private function getArrayDiffParted(array &$from, array &$to): array
    {
        $start = [];
        $end   = [];

        reset($to);

        foreach ($from as $k => $v) {
            $toK = key($to);

            if (($this->isEqual)($toK, $k) && ($this->isEqual)($v, $to[$k])) {
                $start[$k] = $v;

                unset($from[$k], $to[$k]);
            } else {
                break;
            }
        }

        end($from);
        end($to);

        do {
            $fromK = key($from);
            $toK   = key($to);

            if (null === $fromK || null === $toK || !($this->isEqual)(current($from), current($to))) {
                break;
            }

            prev($from);
            prev($to);

            $end = [$fromK => $from[$fromK]] + $end;
            unset($from[$fromK], $to[$toK]);
        } while (true);

        return [$from, $to, $start, $end];
    }

    public function calculateCommonSubsequence(array $from, array $to): array
    {
        $cFrom = count($from);
        $cTo   = count($to);

        if ($cFrom === 0) {
            return [];
        }

        if ($cFrom === 1) {
            foreach ($to as $toV) {
                if (($this->isEqual)($from[0], $toV)) {
                    return [$toV];
                }
            }

            return [];
        }

        $i         = (int) ($cFrom / 2);
        $fromStart = array_slice($from, 0, $i);
        $fromEnd   = array_slice($from, $i);
        $llB       = $this->commonSubsequenceLength($fromStart, $to);
        $llE       = $this->commonSubsequenceLength(array_reverse($fromEnd), array_reverse($to));
        $jMax      = 0;
        $max       = 0;

        for ($j = 0; $j <= $cTo; $j++) {
            $m = $llB[$j] + $llE[$cTo - $j];

            if ($m >= $max) {
                $max  = $m;
                $jMax = $j;
            }
        }

        $toStart = array_slice($to, 0, $jMax);
        $toEnd   = array_slice($to, $jMax);

        return array_merge(
            $this->calculateCommonSubsequence($fromStart, $toStart),
            $this->calculateCommonSubsequence($fromEnd, $toEnd)
        );
    }

    private function commonSubsequenceLength(array $from, array $to): array
    {
        $current = array_fill(0, count($to) + 1, 0);
        $cFrom   = count($from);
        $cTo     = count($to);

        for ($i = 0; $i < $cFrom; $i++) {
            $prev = $current;

            for ($j = 0; $j < $cTo; $j++) {
                if (($this->isEqual)($from[$i], $to[$j])) {
                    $current[$j + 1] = $prev[$j] + 1;
                } else {
                    $current[$j + 1] = max($current[$j], $prev[$j + 1]);
                }
            }
        }

        return $current;
    }
}

class DiffOutputBuilder
{
    public function getDiff(array $diffs): string
    {
        global $context_line_count;
        $i = 0;
        $number_len = max(3, strlen((string)count($diffs)));
        $line_number_spec = '%0' . $number_len . 'd';
        $buffer = fopen('php://memory', 'r+b');
        while ($i < count($diffs)) {
            // Find next difference
            $next = $i;
            while ($next < count($diffs)) {
                if ($diffs[$next][1] !== Differ::OLD) {
                    break;
                }
                $next++;
            }
            // Found no more differentiating rows, we're done
            if ($next === count($diffs)) {
                if (($i - 1) < count($diffs)) {
                    fwrite($buffer, "--\n");
                }
                break;
            }
            // Print separator if necessary
            if ($i < ($next - $context_line_count)) {
                fwrite($buffer, "--\n");
                $i = $next - $context_line_count;
            }
            // Print leading context
            while ($i < $next) {
                fwrite($buffer, str_repeat(' ', $number_len + 2));
                fwrite($buffer, $diffs[$i][0]);
                fwrite($buffer, "\n");
                $i++;
            }
            // Print differences
            while ($i < count($diffs) && $diffs[$i][1] !== Differ::OLD) {
                fwrite($buffer, sprintf($line_number_spec, $diffs[$i][2]));
                switch ($diffs[$i][1]) {
                    case Differ::ADDED:
                        fwrite($buffer, '+ ');
                        break;
                    case Differ::REMOVED:
                        fwrite($buffer, '- ');
                        break;
                }
                fwrite($buffer, $diffs[$i][0]);
                fwrite($buffer, "\n");
                $i++;
            }
            // Print trailing context
            $afterContext = min($i + $context_line_count, count($diffs));
            while ($i < $afterContext && $diffs[$i][1] === Differ::OLD) {
                fwrite($buffer, str_repeat(' ', $number_len + 2));
                fwrite($buffer, $diffs[$i][0]);
                fwrite($buffer, "\n");
                $i++;
            }
        }

        $diff = stream_get_contents($buffer, -1, 0);
        fclose($buffer);

        return $diff;
    }
}

main();
