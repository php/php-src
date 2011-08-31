--TEST--
Bug #45986 (wrong error message for a non existant file on rename)
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich 
--FILE--
<?php
rename('foo', 'bar');
?>
--EXPECTREGEX--
.*No such.*
