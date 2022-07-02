--TEST--
Bug #70943 fopen() can't open a file if path is 259 characters long
--SKIPIF--
<?php
include __DIR__ . DIRECTORY_SEPARATOR . "util.inc";

skip_if_not_win();
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");

?>
--FILE--
<?php
// Generates a sample file whose path is exactly 259 characters long
$testFile = __DIR__ . DIRECTORY_SEPARATOR . str_repeat("a", 254 - strlen(__DIR__)).".dat";
echo "Generating a file with a path length of ".strlen($testFile)." characters...\r\n";
touch($testFile);

echo "Opening file... ";
if ($fp = fopen($testFile, "r")) {
    fclose($fp);
    echo "OK", "\n";
}

unlink($testFile);

?>
--EXPECT--
Generating a file with a path length of 259 characters...
Opening file... OK
