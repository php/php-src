--TEST--
Bug #54028 Directory::read() cannot handle non-unicode chars properly
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
skip_if_no_required_exts();

?>
--FILE--
<?php

/* This file is in UTF-8. */

include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

$prefix = __DIR__ . DIRECTORY_SEPARATOR . "testBug54028" . DIRECTORY_SEPARATOR;

$dirs = array("a", "ソ", "ゾ", "şŞıİğĞ", "多国語", "王", "汚れて掘る");

mkdir($prefix);
foreach ($dirs as $d) {
    mkdir($prefix . $d);
}

$directory = dir($prefix);
while (false !== ($content = $directory->read())) {
    if ("." == $content || ".." == $content) continue;

        printf("Returned (%s)\n", $content);
        printf("Encoding: %s\n", mb_detect_encoding($content));
        if ($content != get_basename_with_cp($prefix . $content, 65001, false)) {
        echo "Verification failed!\n";
    }
    echo "\n";
}

foreach ($dirs as $d) {
    rmdir($prefix . $d);
}
rmdir($prefix);

?>
--EXPECT--
Returned (a)
Encoding: ASCII

Returned (şŞıİğĞ)
Encoding: UTF-8

Returned (ソ)
Encoding: UTF-8

Returned (ゾ)
Encoding: UTF-8

Returned (多国語)
Encoding: UTF-8

Returned (汚れて掘る)
Encoding: UTF-8

Returned (王)
Encoding: UTF-8

