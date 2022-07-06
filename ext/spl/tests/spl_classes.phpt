--TEST--
SPL: spl_classes() function
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich
--FILE--
<?php
var_dump(is_array(spl_classes()));
?>
--EXPECT--
bool(true)
