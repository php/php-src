<?php

if ($argc >= 2) {
    $testsDir = $argv[1];
} else {
    $testsDir = __DIR__ . '/../../Zend/tests';
}
if ($argc >= 3) {
    $corpusDir = $argv[2];
} else {
    $corpusDir = __DIR__ . '/corpus/execute';
}
if ($argc >= 4) {
    $maxLen = (int) $argv[3];
} else {
    $maxLen = 8 * 1024;
}

$it = new RecursiveIteratorIterator(
    new RecursiveDirectoryIterator($testsDir),
    RecursiveIteratorIterator::LEAVES_ONLY
);

@mkdir($corpusDir);

foreach ($it as $file) {
    if (!preg_match('/\.phpt$/', $file)) continue;
    $fullCode = file_get_contents($file);
    if (!preg_match('/--FILE--\R(.*?)\R--([_A-Z]+)--/s', $fullCode, $matches)) continue;
    $code = $matches[1];
    if (strlen($code) > $maxLen) continue;

    $outFile = str_replace($testsDir, '', $file);
    $outFile = str_replace('/', '_', $outFile);
    if (!preg_match('/SKIP_SLOW_TESTS|SKIP_PERF_SENSITIVE|USE_ZEND_ALLOC/', $fullCode)) {
        file_put_contents($corpusDir . '/' . $outFile, $code);
    }
}
