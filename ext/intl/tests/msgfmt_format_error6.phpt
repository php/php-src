--TEST--
MessageFormatter::format() invalid type for key not in pattern
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '4.8') < 0)
	die('skip for ICU 4.8+');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$fmt = <<<EOD
{foo}
EOD;

$mf = new MessageFormatter('en_US', $fmt);
var_dump($mf->format(array("foo" => 'bar', 7 => fopen('php://memory', 'r+'))));
--EXPECTF--
Warning: MessageFormatter::format(): No strategy to convert the value given for the argument with key '7' is available in %s on line %d
bool(false)
