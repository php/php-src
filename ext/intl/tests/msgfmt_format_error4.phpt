--TEST--
MessageFormatter::format() invalid UTF-8 for arg key or value
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
var_dump($mf->format(array("foo" => 7, "\x80" => "bar")));

var_dump($mf->format(array("foo" => "\x80")));

--EXPECTF--

Warning: MessageFormatter::format(): Invalid UTF-8 data in argument key: '€' in %s on line %d
bool(false)

Warning: MessageFormatter::format(): Invalid UTF-8 data in string argument: '€' in %s on line %d
bool(false)
