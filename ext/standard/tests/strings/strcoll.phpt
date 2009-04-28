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
--EXPECT--
C: 32
