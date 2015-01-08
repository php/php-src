--TEST--
Bug #68636 (setlocale no longer returns current value per category).
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not valid for windows');
}
if (setlocale(LC_ALL, "en_US.UTF8") !== "en_US.UTF8") {
    die('skip available locales not usable');
}
?>
--FILE--
<?php
var_dump(setlocale(LC_TIME, 'en_US.UTF8'));
var_dump(setlocale(LC_NUMERIC, 'C'));
var_dump(setlocale(LC_TIME, 0));
?>
--EXPECT--
string(10) "en_US.UTF8"
string(1) "C"
string(10) "en_US.UTF8"
