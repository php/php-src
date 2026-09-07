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
--EXPECTF--
Deprecated: Function spl_classes() is deprecated since 8.6, use ReflectionExtension::getClassNames() instead in %s on line %d
bool(true)
