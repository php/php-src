#!/usr/bin/env php
<?php

if ($argc < 2) {
    die("Usage: php bless_tests.php dir/\n");
}

$files = getFiles(array_slice($argv, 1));
foreach ($files as $path) {
    if (!preg_match('/^(.*)\.phpt$/', $path, $matches)) {
        // Not a phpt test
        continue;
    }

    $outPath = $matches[1] . '.out';
    if (!file_exists($outPath)) {
        // Test did not fail
        continue;
    }

    $phpt = file_get_contents($path);
    if (false !== strpos($phpt, '--XFAIL--')) {
        // Don't modify expected output of XFAIL tests
        continue;
    }

    $out = file_get_contents($outPath);
    $out = normalizeOutput($out);
    $phpt = insertOutput($phpt, $out);
    file_put_contents($path, $phpt);
}

function getFiles(array $dirsOrFiles): \Iterator {
    foreach ($dirsOrFiles as $dirOrFile) {
        if (is_dir($dirOrFile)) {
            $it = new RecursiveIteratorIterator(
                new RecursiveDirectoryIterator($dirOrFile),
                RecursiveIteratorIterator::LEAVES_ONLY
            );
            foreach ($it as $file) {
                yield $file->getPathName();
            }
        } else if (is_file($dirOrFile)) {
            yield $dirOrFile;
        } else {
            die("$dirOrFile is not a directory or file\n");
        }
    }
}

function normalizeOutput(string $out): string {
    $out = preg_replace('/in \/.+ on line \d+$/m', 'in %s on line %d', $out);
    $out = preg_replace('/in \/.+:\d+$/m', 'in %s:%d', $out);
    $out = preg_replace('/^#(\d+) \/.+\(\d+\):/m', '#$1 %s(%d):', $out);
    $out = preg_replace('/Resource id #\d+/', 'Resource id #%d', $out);
    $out = preg_replace('/resource\(\d+\) of type/', 'resource(%d) of type', $out);
    $out = preg_replace(
        '/Resource ID#\d+ used as offset, casting to integer \(\d+\)/',
        'Resource ID#%d used as offset, casting to integer (%d)',
        $out);
    $out = preg_replace('/string\(\d+\) "([^"]*%d)/', 'string(%d) "$1', $out);
    return $out;
}

function insertOutput(string $phpt, string $out): string {
    return preg_replace_callback('/--EXPECTF?--.*$/s', function($matches) use($out) {
        $F = strpos($out, '%') !== false ? 'F' : '';
        return "--EXPECT$F--\n" . $out . "\n";
    }, $phpt);
}
