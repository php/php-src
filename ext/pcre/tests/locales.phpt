--TEST--
Localized match
--SKIPIF--
<?php

if (!@setlocale(LC_ALL, 'pt_PT', 'pt', 'pt_PT.ISO8859-1', 'portuguese')) die('skip pt locale not available');

?>
--FILE--
<?php

// this tests if the cache is working correctly, as the char tables
// must be rebuilt after the locale change

setlocale(LC_ALL, 'C', 'POSIX');
var_dump(preg_match('/^\w{6}$/', 'a�����'));

setlocale(LC_ALL, 'pt_PT', 'pt', 'pt_PT.ISO8859-1', 'portuguese');
var_dump(preg_match('/^\w{6}$/', 'a�����'));

setlocale(LC_ALL, 'C', 'POSIX');
var_dump(preg_match('/^\w{6}$/', 'a�����'));

?>
--EXPECT--
int(0)
int(1)
int(0)
