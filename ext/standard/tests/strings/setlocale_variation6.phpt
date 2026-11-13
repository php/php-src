--TEST--
Test setlocale() function : usage variations - restoring the name of LC_ALL
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not valid for windows');
}
if (setlocale(LC_CTYPE, 'en_US.UTF-8', 'en_US.utf8', 'C.UTF-8') === false) {
    die('skip no UTF-8 locale available');
}
?>
--FILE--
<?php
/* When the categories have different locales, the name of LC_ALL is made of
 * their names, and restores the locale of each category */

setlocale(LC_ALL, 'C');
$ctype = setlocale(LC_CTYPE, 'en_US.UTF-8', 'en_US.utf8', 'C.UTF-8');
$all = setlocale(LC_ALL, 0);
var_dump($all !== 'C' && $all !== $ctype);

setlocale(LC_ALL, 'C');
var_dump(setlocale(LC_ALL, 0));

var_dump(setlocale(LC_ALL, $all) === $all);
var_dump(setlocale(LC_CTYPE, 0) === $ctype);
var_dump(setlocale(LC_NUMERIC, 0));
var_dump(setlocale(LC_ALL, 0) === $all);
?>
--EXPECT--
bool(true)
string(1) "C"
bool(true)
bool(true)
string(1) "C"
bool(true)
