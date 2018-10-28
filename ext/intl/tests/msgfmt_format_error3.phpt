--TEST--
MessageFormatter::format() given negative arg key
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$fmt = <<<EOD
{foo,number,percent}
EOD;

$mf = new MessageFormatter('en_US', $fmt);
var_dump($mf->format(array("foo" => 7, -1 => "bar")));
--EXPECTF--
Warning: MessageFormatter::format(): Found negative or too large array key in %s on line %d
bool(false)
