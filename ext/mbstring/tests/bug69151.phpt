--TEST--
Bug #69151 (mb_ereg should reject ill-formed byte sequence)
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip mbstring not available');
if (!function_exists('mb_ereg')) die('skip mbregex support not available');
?>
--FILE--
<?php
$str = "\x80";

try {
    var_dump(false === mb_eregi('.', $str, $matches));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump([] === $matches);

try {
    var_dump(NULL === mb_ereg_replace('.', "\\0", $str));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(false === mb_ereg_search_init("\x80", '.'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(false === mb_ereg_search());
?>
--EXPECT--
mb_eregi(): Argument #2 ($string) must be a valid string in 'UTF-8'
bool(true)
mb_ereg_replace(): Argument #3 ($string) must be a valid string in 'UTF-8'
mb_ereg_search_init(): Argument #1 ($string) must be a valid string in 'UTF-8'
bool(true)
