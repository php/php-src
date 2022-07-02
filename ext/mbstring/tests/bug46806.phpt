--TEST--
Bug #46806 (mb_strimwidth cutting too early)
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009
--EXTENSIONS--
mbstring
--FILE--
<?php
echo mb_strimwidth('helloworld', 0, 5, '...', 'UTF-8') . "\n";
echo mb_strimwidth('hello', 0, 5, '...', 'UTF-8');
?>
--EXPECT--
he...
hello
