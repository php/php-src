<?php

const BRANCHES = ['master', 'PHP-8.4', 'PHP-8.3', 'PHP-8.2', 'PHP-8.1'];

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
        $previous_commit_hash = $branch_commit_map[$branch] ?? null;
        $current_commit_hash = trim(shell_exec('git rev-parse origin/' . $branch));

        if ($previous_commit_hash !== $current_commit_hash) {
            $changed_branches[] = $branch;
        }

        $branch_commit_map[$branch] = $current_commit_hash;
    }

    file_put_contents($branch_commit_cache_file, json_encode($branch_commit_map));

    return $changed_branches;
}

$trigger = $argv[1] ?? 'schedule';
$attempt = (int) ($argv[2] ?? 1);
$monday = date('w', time()) === '1';
$discard_cache = $monday
    || ($trigger === 'schedule' && $attempt !== 1)
    || $trigger === 'workflow_dispatch';
if ($discard_cache) {
    @unlink(get_branch_commit_cache_file_path());
}
$branch = $argv[3] ?? 'master';
$branches = $branch === 'master' ? get_branches() : [$branch];

$f = fopen(getenv('GITHUB_OUTPUT'), 'a');
fwrite($f, 'branches=' . json_encode($branches, JSON_UNESCAPED_SLASHES) . "\n");
fclose($f);
