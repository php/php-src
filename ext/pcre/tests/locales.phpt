--TEST--
Localized match
--SKIPIF--
<?php if (!function_exists('setlocale')) die('skip: setlocale() not available'); ?>
<?php if (!@setlocale(LC_ALL, 'pt_PT', 'pt', 'pt_PT.ISO8859-1', 'portuguese')) die('skip pt locale not available');
--FILE--
<?php

declare(encoding='latin1');

// this tests if the cache is working correctly, as the char tables
// must be rebuilt after the locale change

setlocale(LC_ALL, 'C', 'POSIX');
var_dump(preg_match('/^\w{6}$/', 'aאבחיט'));

setlocale(LC_ALL, 'pt_PT', 'pt', 'pt_PT.ISO8859-1', 'portuguese');
var_dump(preg_match('/^\w{6}$/', 'aאבחיט'));

setlocale(LC_ALL, 'C', 'POSIX');
var_dump(preg_match('/^\w{6}$/', 'aאבחיט'));

?>
--EXPECT--
Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
int(0)

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
int(1)

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
int(0)
