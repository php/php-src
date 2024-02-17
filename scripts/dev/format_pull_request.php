<?php

declare(strict_types=1);
error_reporting(E_ALL);
set_error_handler(function ($errno, $errstr, $errfile, $errline) {
    if (error_reporting() & $errno) {
        throw new ErrorException($errstr, 0, $errno, $errfile, $errline);
    }
});
$force = array_search('--force', $argv, true);
if ($force !== false) {
    unset($argv[$force]);
    $force = true;
    $argv = array_values($argv);
    $argc = count($argv);
}
if ($argc !== 2) {
    ob_start();
    echo "Usage: php {$argv[0]} [diff-uri]\n";
    echo "example:\nphp {$argv[0]} " . escapeshellarg('https://github.com/php/php-src/pull/13401.diff') . "\n";
    fwrite(STDERR, ob_get_clean());
    exit(1);
}
chdir(__DIR__ . '/../../'); // php-src root
if (!$force) {
    $gitStatus = null;
    system('git diff --quiet --exit-code', $gitStatus);
    if ($gitStatus !== 0) {
        fwrite(STDERR, "Error: Working directory is not clean according to 'git diff'\n");
        fwrite(STDERR, "commit your stuff -or- run 'git reset --hard' and try again\n");
        exit(1);
    }
    unset($gitStatus);
}
//
$diff = file_get_contents($argv[1]);
$diffFileHandle = tmpfile();
fwrite($diffFileHandle, $diff);
$diffFilePath = stream_get_meta_data($diffFileHandle)['uri'];
passthru("git apply " . escapeshellarg($diffFilePath), $ret);
if ($ret !== 0) {
    fwrite(STDERR, "Error: git apply failed with exit code {$ret}\n");
    if (!$force) {
        passthru('git reset --hard');
    }
    exit(1);
}
// commit
passthru('git commit -a --message=clangformat --quiet', $ret);
if ($ret !== 0) {
    fwrite(STDERR, "Error: git commit failed with exit code {$ret}\n");
    if (!$force) {
        passthru('git reset --hard');
    }
    exit(1);
}
$clangFormatOptions = array(
    // tests on ext/standard/array.c indicate that the clang-format
    // preset that most closely resembles the php-src coding style is WebKit
    'BasedOnStyle' => 'WebKit',
    'UseTab' => 'ForIndentation',
    // clang-format's support for tabs isn't great, and we have to say "4 spaces" to get 1 tab
    'IndentWidth' => 4,
    'TabWidth' => 4,
);
$cmd = array(
    'git diff -U0 --no-color --relative HEAD^ |',
    'clang-format-diff -i -p1 -style=' . escapeshellarg(json_encode($clangFormatOptions)),
    '-iregex=' . escapeshellarg('.*\.(c|cc|cpp|c\+\+|cxx|h|hh|hpp|h\+\+|h)$'),
);
$cmd = implode(' ', $cmd);
passthru($cmd, $ret);
if ($ret !== 0) {
    fwrite(STDERR, "Error: clang-format-diff failed with exit code {$ret}\n");
    fwrite(STDERR, "cmd:\n{$cmd}\n");
    if (!$force) {
        // remove last commit and reset
        passthru('git reset --hard HEAD^ --quiet');
    }
    exit(1);
}
passthru('git diff');
if (!$force) {
    // remove last commit and reset
    passthru('git reset --hard HEAD^ --quiet');
}
