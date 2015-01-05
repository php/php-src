--TEST--
Bug #46806 (mb_wtrimwidth cutting to early)
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 
--SKIPIF--
<?php if (!extension_loaded("mbstring")) die("skip mbstring is not available"); ?>
--FILE--
<?php
echo mb_strimwidth('helloworld', 0, 5, '...', 'UTF-8') . "\n";
echo mb_strimwidth('hello', 0, 5, '...', 'UTF-8');
?>
--EXPECT--
he...
hello
