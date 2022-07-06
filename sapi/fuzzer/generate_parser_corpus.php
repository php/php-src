<?php

$testsDir = __DIR__ . '/../../Zend/tests/';
$it = new RecursiveIteratorIterator(
    new RecursiveDirectoryIterator($testsDir),
    RecursiveIteratorIterator::LEAVES_ONLY
);

$corpusDir = __DIR__ . '/corpus/parser';
@mkdir($corpusDir);

$maxLen = 32 * 1024;
foreach ($it as $file) {
    if (!preg_match('/\.phpt$/', $file)) continue;
    $code = file_get_contents($file);
    if (!preg_match('/--FILE--\R(.*?)\R--([_A-Z]+)--/s', $code, $matches)) continue;
    $code = $matches[1];
    if (strlen($code) > $maxLen) continue;

    $outFile = str_replace($testsDir, '', $file);
    $outFile = str_replace('/', '_', $outFile);
    $outFile = $corpusDir . '/' . $outFile;
    file_put_contents($outFile, $code);
}
