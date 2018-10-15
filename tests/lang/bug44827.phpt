--TEST--
Bug #44827 (Class error when trying to access :: as constant)
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest Munich 2009
--FILE--
<?php
define('::', true);
var_dump(constant('::'));
?>
--EXPECTF--
Warning: Class constants cannot be defined or redefined in %s on line %d

Warning: constant(): Couldn't find constant :: in %s on line %d
NULL
