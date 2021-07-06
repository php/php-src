<?php

if ($argc >= 2) {
    $corpusDir = $argv[1];
} else {
    $corpusDir = __DIR__ . '/corpus/execute';
}
if ($argc >= 3) {
    $testDirs = array_slice($argv, 2);
} else {
    $baseDir = __DIR__ . '/../..';
    $testDirs = ["$baseDir/Zend/tests", "$baseDir/ext/reflection"];
}

$maxLen = 8 * 1024;
@mkdir($corpusDir);

foreach ($testDirs as $testDir) {
    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($testDir),
        RecursiveIteratorIterator::LEAVES_ONLY
    );

    foreach ($it as $file) {
        if (!preg_match('/\.phpt$/', $file)) continue;
        $fullCode = file_get_contents($file);
        if (!preg_match('/--FILE--\R(.*?)\R--([_A-Z]+)--/s', $fullCode, $matches)) continue;
        $code = $matches[1];
        if (strlen($code) > $maxLen) continue;

        $outFile = str_replace($testDir, '', $file);
        $outFile = str_replace('/', '_', $outFile);
        if (!preg_match('/SKIP_SLOW_TESTS|SKIP_PERF_SENSITIVE|USE_ZEND_ALLOC/', $fullCode)) {
            file_put_contents($corpusDir . '/' . $outFile, $code);
        }
    }
}
