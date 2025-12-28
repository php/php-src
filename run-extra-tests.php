#!/usr/bin/env php
<?php
/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
 */

/* This is a single entrypoint for non-phpt tests. */

class Environment
{
    public function __construct(
        public string $os,
        public string $cpuArch,
        public bool $zts,
        public bool $debug,
        public bool $githubAction,
    ) {}
}

function show_usage(): void
{
    echo <<<HELP
    Synopsis:
        php run-extra-tests.php

    Environment variables:
        TEST_PHP_OS:       One of 'Windows NT', 'Linux', 'FreeBSD', 'Darwin'
        TEST_PHP_CPU_ARCH: One of 'x86', 'x86_64', 'aarch64'

    HELP;
}

function main(int $argc, array $argv): void
{
    if ($argc !== 1) {
        show_usage();
        exit(1);
    }

    $environment = new Environment(
        detect_os(),
        detect_cpu_arch(),
        PHP_ZTS,
        PHP_DEBUG,
        getenv('GITHUB_ACTIONS') === 'true',
    );

    echo "=====================================================================\n";
    echo "OS:       {$environment->os}\n";
    echo "CPU Arch: {$environment->cpuArch}\n";
    echo "ZTS:      " . ($environment->zts ? 'Yes' : 'No') . "\n";
    echo "DEBUG:    " . ($environment->debug ? 'Yes' : 'No') . "\n";
    echo "=====================================================================\n";

    try {
        output_group_start($environment, 'Running Opcache TLS tests');
        if (!run_opcache_tls_tests($environment)) {
            echo "Failed\n";
            exit(1);
        }
    } finally {
        output_group_end($environment);
    }

    echo "All OK\n";
}

function run_opcache_tls_tests(Environment $environment): bool
{
    if (!$environment->zts) {
        echo "Skipping: NTS\n";
        return true;
    }

    if (!$environment->debug) {
        echo "Skipping: NDEBUG\n";
        return true;
    }

    $tlsc = '';
    $machine = '';
    $static_support = 'yes';

    switch ($environment->cpuArch) {
        case 'x86':
            $machine = '-m32';
            break;
        case 'x86_64':
        case 'aarch64':
            break;
        default:
            echo "Skipping: {$environment->cpuArch}\n";
            return true;
    }

    switch ($environment->os) {
        case 'Linux':
        case 'FreeBSD':
            $tlsc = __DIR__ . "/ext/opcache/jit/tls/zend_jit_tls_{$environment->cpuArch}.c";
            break;
        case 'Darwin':
            if ($environment->cpuArch === 'aarch64') {
                echo "Skipping: JIT+TLS not supported on MacOS Apple Silicon\n";
                return true;
            }
            $tlsc = __DIR__ . "/ext/opcache/jit/tls/zend_jit_tls_darwin.c";
            $static_support = 'no';
            break;
        default:
            echo "Skipping: {$environment->os}\n";
            return true;
    }

    echo "TLSC=$tlsc MACHINE=$machine STATIC_SUPPORT=$static_support ext/opcache/jit/tls/testing/test.sh\n";

    $proc = proc_open(
        __DIR__ . '/ext/opcache/jit/tls/testing/test.sh',
        [
            0 => ['pipe', 'r'],
        ],
        $pipes,
        env_vars: array_merge(getenv(), [
            'TLSC' => $tlsc,
            'MACHINE' => $machine,
            'STATIC_SUPPORT' => $static_support,
        ]),
    );

    if (!$proc) {
        echo "proc_open() failed\n";
        return false;
    }

    fclose($pipes[0]);

    return proc_close($proc) === 0;
}

function output_group_start(Environment $environment, string $name): void
{
    if ($environment->githubAction) {
        printf("::group::%s\n", $name);
    } else {
        printf("%s\n", $name);
    }
}

function output_group_end(Environment $environment): void
{
    if ($environment->githubAction) {
        printf("::endgroup::\n");
    }
}

/**
 * Returns getenv('TEST_PHP_OS') if defined, otherwise returns one of
 * 'Windows NT', 'Linux', 'FreeBSD', 'Darwin', ...
 */
function detect_os(): string
{
    $os = (string) getenv('TEST_PHP_OS');
    if ($os !== '') {
        return $os;
    }

    return php_uname('s');
}

/**
 * Returns getenv('TEST_PHP_CPU_ARCH') if defined, otherwise returns one of
 * 'x86', 'x86_64', 'aarch64', ...
 */
function detect_cpu_arch(): string
{
    $cpu = (string) getenv('TEST_PHP_CPU_ARCH');
    if ($cpu !== '') {
        return $cpu;
    }

    $cpu = php_uname('m');
    if (strtolower($cpu) === 'amd64') {
        $cpu = 'x86_64';
    } else if (in_array($cpu, ['i386', 'i686'])) {
        $cpu = 'x86';
    } else if ($cpu === 'arm64') {
        $cpu = 'aarch64';
    }

    return $cpu;
}

main($argc, $argv);
