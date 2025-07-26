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

    echo "No tests in this branch yet.\n";

    echo "All OK\n";
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
