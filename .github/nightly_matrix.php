<?php

const BRANCHES = [
    ['ref' => 'master', 'version' => [8, 5]],
    ['ref' => 'PHP-8.4', 'version' => [8, 4]],
    ['ref' => 'PHP-8.3', 'version' => [8, 3]],
    ['ref' => 'PHP-8.2', 'version' => [8, 2]],
    ['ref' => 'PHP-8.1', 'version' => [8, 1]],
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

function get_current_version(): array {
    $file = dirname(__DIR__) . '/main/php_version.h';
    $content = file_get_contents($file);
    preg_match('(^#define PHP_MAJOR_VERSION (?<num>\d+)$)m', $content, $matches);
    $major = (int) $matches['num'];
    preg_match('(^#define PHP_MINOR_VERSION (?<num>\d+)$)m', $content, $matches);
    $minor = (int) $matches['num'];
    return [$major, $minor];
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
$branches = $branch === 'master'
    ? get_branches()
    : [['ref' => $branch, 'version' => get_current_version()]];

$f = fopen(getenv('GITHUB_OUTPUT'), 'a');
fwrite($f, 'branches=' . json_encode($branches, JSON_UNESCAPED_SLASHES) . "\n");
fclose($f);
