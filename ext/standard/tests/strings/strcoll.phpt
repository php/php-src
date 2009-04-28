--TEST--
Testing Basic behaviour of strcoll()
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich
--FILE--
<?php

 $a = 'a';
 $b = 'A';

setlocale (LC_COLLATE, 'C');
print "C: " . strcoll ($a, $b) . "\n"; // prints 32
?>
--EXPECTF--
Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
C: -1
