<?php
require __DIR__ . '/generate_unserialize_dict.php';
require __DIR__ . '/generate_mbstring_dict.php';
require __DIR__ . '/generate_unserializehash_corpus.php';
require __DIR__ . '/generate_parser_corpus.php';

require __DIR__ . '/generate_corpus_util.php';

$baseDir = __DIR__ . '/../..';
generate_corpus_from_phpt(
    __DIR__ . '/corpus/execute',
    ["$baseDir/Zend/tests", "$baseDir/ext/reflection"]
);
generate_corpus_from_phpt(
    __DIR__ . '/corpus/function-jit',
    ["$baseDir/Zend/tests", "$baseDir/ext/opcache/tests/jit"]
);
generate_corpus_from_phpt(
    __DIR__ . '/corpus/tracing-jit',
    ["$baseDir/Zend/tests", "$baseDir/ext/opcache/tests/jit"]
);
