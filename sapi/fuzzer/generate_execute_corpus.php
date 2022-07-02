<?php

require __DIR__ . '/generate_corpus_util.php';

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

generate_corpus_from_phpt($corpusDir, $testDirs);
