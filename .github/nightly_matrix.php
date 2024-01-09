<?php

const BRANCHES = [
    ['name' => 'master', 'ref' => 'master', 'version' => ['major' => 8, 'minor' => 4]],
    ['name' => 'PHP-8.3', 'ref' => 'PHP-8.3', 'version' => ['major' => 8, 'minor' => 3]],
    ['name' => 'PHP-8.2', 'ref' => 'PHP-8.2', 'version' => ['major' => 8, 'minor' => 2]],
    ['name' => 'PHP-8.1', 'ref' => 'PHP-8.1', 'version' => ['major' => 8, 'minor' => 1]],
];

function get_branch_commit_cache_file_path(): string {
    return dirname(__DIR__) . '/branch-commit-cache.json';
}

function get_branches() {
    $branch_commit_cache_file = get_branch_commit_cache_file_path();
    $branch_commit_map = [];
    if (file_exists($branch_commit_cache_file)) {
        $branch_commit_map = json_decode(file_get_contents($branch_commit_cache_file), JSON_THROW_ON_ERROR);
    }

    $changed_branches = [];
    foreach (BRANCHES as $branch) {
        $previous_commit_hash = $branch_commit_map[$branch['ref']] ?? null;
        $current_commit_hash = trim(shell_exec('git rev-parse origin/' . $branch['ref']));

        if ($previous_commit_hash !== $current_commit_hash) {
            $changed_branches[] = $branch;
        }

        $branch_commit_map[$branch['ref']] = $current_commit_hash;
    }

    file_put_contents($branch_commit_cache_file, json_encode($branch_commit_map));

    return $changed_branches;
}

function get_matrix_include(array $branches) {
    $jobs = [];
    foreach ($branches as $branch) {
        $jobs[] = [
            'name' => '_ASAN_UBSAN',
            'branch' => $branch,
            'debug' => true,
            'zts' => true,
            'configuration_parameters' => "CFLAGS='-fsanitize=undefined,address -DZEND_TRACK_ARENA_ALLOC' LDFLAGS='-fsanitize=undefined,address'",
            'run_tests_parameters' => '--asan',
            'test_function_jit' => false,
            'asan' => true,
        ];
        $jobs[] = [
            'name' => '_REPEAT',
            'branch' => $branch,
            'debug' => true,
            'zts' => false,
            'run_tests_parameters' => '--repeat 2',
            'timeout_minutes' => 360,
            'test_function_jit' => true,
            'asan' => false,
        ];
        $jobs[] = [
            'name' => '_VARIATION',
            'branch' => $branch,
            'debug' => true,
            'zts' => true,
            'configuration_parameters' => "CFLAGS='-DZEND_RC_DEBUG=1 -DPROFITABILITY_CHECKS=0 -DZEND_VERIFY_FUNC_INFO=1 -DZEND_VERIFY_TYPE_INFERENCE'",
            'timeout_minutes' => 360,
            'test_function_jit' => true,
            'asan' => false,
        ];
    }
    return $jobs;
}

function get_windows_matrix_include(array $branches) {
    $jobs = [];
    foreach ($branches as $branch) {
        $jobs[] = [
            'branch' => $branch,
            'x64' => true,
            'zts' => true,
            'opcache' => true,
        ];
        $jobs[] = [
            'branch' => $branch,
            'x64' => false,
            'zts' => false,
            'opcache' => false,
        ];
    }
    return $jobs;
}

function get_current_version(): array {
    $file = dirname(__DIR__) . '/main/php_version.h';
    $content = file_get_contents($file);
    preg_match('(^#define PHP_MAJOR_VERSION (?<num>\d+)$)m', $content, $matches);
    $major = $matches['num'];
    preg_match('(^#define PHP_MINOR_VERSION (?<num>\d+)$)m', $content, $matches);
    $minor = $matches['num'];
    return ['major' => $major, 'minor' => $minor];
}

$trigger = $argv[1] ?? 'schedule';
$attempt = (int) ($argv[2] ?? 1);
$discard_cache = ($trigger === 'schedule' && $attempt !== 1) || $trigger === 'workflow_dispatch';
if ($discard_cache) {
    @unlink(get_branch_commit_cache_file_path());
}
$branch = $argv[3] ?? 'master';

$branches = $branch === 'master'
    ? get_branches()
    : [['name' => strtoupper($branch), 'ref' => $branch, 'version' => get_current_version()]];
$matrix_include = get_matrix_include($branches);
$windows_matrix_include = get_windows_matrix_include($branches);

$f = fopen(getenv('GITHUB_OUTPUT'), 'a');
fwrite($f, 'branches=' . json_encode($branches, JSON_UNESCAPED_SLASHES) . "\n");
fwrite($f, 'matrix-include=' . json_encode($matrix_include, JSON_UNESCAPED_SLASHES) . "\n");
fwrite($f, 'windows-matrix-include=' . json_encode($windows_matrix_include, JSON_UNESCAPED_SLASHES) . "\n");
fclose($f);
