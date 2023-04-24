--TEST--
Trying to parse a file that is too large (over 4GB)
--EXTENSIONS--
tidy
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (getenv("SKIP_ASAN")) die("skip too big for asan");
?>
--CONFLICTS--
all
--INI--
memory_limit="5G"
--FILE--
<?php

$path = __DIR__ . '/too_large_test.html';
$file = fopen($path, 'w+');

// Write over 4GB
const MIN_FILE_SIZE = 4_294_967_295;

var_dump(fseek($file, MIN_FILE_SIZE+10));
$s = str_repeat("a", 10);
$bytes_written = fwrite($file, $s);
if ($bytes_written === false) {
    echo "Didn't write bytes\n";
}

$tidy = new tidy;
try {
    var_dump($tidy->parseFile($path));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump(tidy_parse_file($path));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $tidy = new tidy($path);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--CLEAN--
<?php
$path = __DIR__ . '/too_large_test.html';
unlink($path);
?>
--EXPECT--
int(0)
ValueError: Input string is too long
ValueError: Input string is too long
ValueError: Input string is too long
