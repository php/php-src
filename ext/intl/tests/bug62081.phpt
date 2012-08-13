--TEST--
Bug #62081: IntlDateFormatter leaks memory if called twice
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set('intl.error_level', E_WARNING);
$x = new IntlDateFormatter('en', 1, 1);
var_dump($x->__construct('en', 1, 1));
--EXPECTF--
Warning: IntlDateFormatter::__construct(): datefmt_create: cannot call constructor twice in %s on line %d
NULL
