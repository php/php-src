<?php

const BRANCHES = ['master', 'PHP-8.1', 'PHP-8.0'];

function get_branch_commit_cache_file_path(): string {
    return dirname(__DIR__) . '/branch-commit-cache.json';
}

function get_branch_matrix(array $branches) {
    $result = array_map(function ($branch) {
        $branch_key = strtoupper(str_replace('.', '', $branch));
        return [
            'name' => $branch_key,
            'ref' => $branch,
        ];
    }, $branches);

    return $result;
}

function get_branches() {
    $branch_commit_cache_file = get_branch_commit_cache_file_path();
    $branch_commit_map = [];
    if (file_exists($branch_commit_cache_file)) {
        $branch_commit_map = json_decode(file_get_contents($branch_commit_cache_file), JSON_THROW_ON_ERROR);
    }

    $changed_branches = [];
    foreach (BRANCHES as $branch) {
        $previous_commit_hash = $branch_commit_map[$branch] ?? null;
        $current_commit_hash = trim(shell_exec('git rev-parse origin/' . $branch));

        if ($previous_commit_hash !== $current_commit_hash) {
            $changed_branches[] = $branch;
        }

        $branch_commit_map[$branch] = $current_commit_hash;
    }

    file_put_contents($branch_commit_cache_file, json_encode($branch_commit_map));

    return get_branch_matrix($changed_branches);
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
        ];
    }
    return $jobs;
}

$trigger = $argv[1] ?? 'schedule';
$attempt = (int) ($argv[2] ?? 1);
$discard_cache = ($trigger === 'schedule' && $attempt !== 1) || $trigger === 'workflow_dispatch';
if ($discard_cache) {
    @unlink(get_branch_commit_cache_file_path());
}

$branches = get_branches();
$matrix_include = get_matrix_include($branches);

echo '::set-output name=branches::' . json_encode($branches, JSON_UNESCAPED_SLASHES) . "\n";
echo '::set-output name=matrix-include::' . json_encode($matrix_include, JSON_UNESCAPED_SLASHES) . "\n";
