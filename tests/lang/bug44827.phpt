--TEST--
Bug #44827 (Class error when trying to access :: as constant)
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest Munich 2009
--FILE--
<?php

require __DIR__ . '/../../Zend/tests/constants_helpers.inc';

tchelper_define('::', true);

?>
--EXPECT--
>> define("::", boolean);
ValueError :: Class constants cannot be defined or redefined
