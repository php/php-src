--TEST--
Localized match
--SKIPIF--
<?php if (!function_exists('setlocale')) die('setlocale() not available'); ?>
--FILE--
<?php

// this tests if the cache is working correctly, as the char tables
// must be rebuilt after the locale change

setlocale(LC_ALL, 'C', 'POSIX');
var_dump(preg_match('/^\w{6}$/', 'aאבחיט'));

setlocale(LC_ALL, 'pt_PT', 'pt', 'portuguese');
var_dump(preg_match('/^\w{6}$/', 'aאבחיט'));

setlocale(LC_ALL, 'C', 'POSIX');
var_dump(preg_match('/^\w{6}$/', 'aאבחיט'));

?>
--EXPECT--
int(0)
int(1)
int(0)
