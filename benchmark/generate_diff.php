<?php

require_once __DIR__ . '/shared.php';

function main(?string $headCommitHash, ?string $baseCommitHash) {
    if ($headCommitHash === null || $baseCommitHash === null) {
        fwrite(STDERR, "Usage: php generate_diff.php HEAD_COMMIT_HASH BASE_COMMIT_HASH\n");
        exit(1);
    }

    $repo = __DIR__ . '/repos/data';
    cloneRepo($repo, 'git@github.com:php/benchmarking-data.git');
    $headSummaryFile = $repo . '/' . substr($headCommitHash, 0, 2) . '/' . $headCommitHash . '/summary.json';
    $baseSummaryFile = $repo . '/' . substr($baseCommitHash, 0, 2) . '/' . $baseCommitHash . '/summary.json';
    if (!file_exists($headSummaryFile)) {
        return "Head commit '$headCommitHash' not found\n";
    }
    if (!file_exists($baseSummaryFile)) {
        return "Base commit '$baseCommitHash' not found\n";
    }
    $headSummary  = json_decode(file_get_contents($headSummaryFile), true);
    $baseSummary  = json_decode(file_get_contents($baseSummaryFile), true);

    $headCommitHashShort = substr($headCommitHash, 0, 7);
    $baseCommitHashShort = substr($baseCommitHash, 0, 7);
    $output = "| Benchmark | Base ($baseCommitHashShort) | Head ($headCommitHashShort) | Diff |\n";
    $output .= "|---|---|---|---|\n";
    foreach ($headSummary as $name => $headBenchmark) {
        if ($name === 'branch') {
            continue;
        }
        $baseInstructions = $baseSummary[$name]['instructions'] ?? null;
        $headInstructions = $headSummary[$name]['instructions'];
        $output .= "| $name | "
            . formatInstructions($baseInstructions) . " | "
            . formatInstructions($headInstructions) . " | "
            . formatDiff($baseInstructions, $headInstructions) . " |\n";
    }
    return $output;
}

function formatInstructions(?int $instructions): string {
    if ($instructions === null) {
        return '-';
    }
    if ($instructions > 1e6) {
        return sprintf('%.0fM', $instructions / 1e6);
    } elseif ($instructions > 1e3) {
        return sprintf('%.0fK', $instructions / 1e3);
    } else {
        return (string) $instructions;
    }
}

function formatDiff(?int $baseInstructions, int $headInstructions): string {
    if ($baseInstructions === null) {
        return '-';
    }
    $instructionDiff = $headInstructions - $baseInstructions;
    return sprintf('%.2f%%', $instructionDiff / $baseInstructions * 100);
}

$headCommitHash = $argv[1] ?? null;
$baseCommitHash = $argv[2] ?? null;
$output = main($headCommitHash, $baseCommitHash);
fwrite(STDOUT, $output);
