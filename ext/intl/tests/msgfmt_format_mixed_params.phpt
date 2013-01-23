--TEST--
MessageFormatter::format(): mixed named and numeric parameters
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
if (version_compare(INTL_ICU_VERSION, '4.8') < 0)
	die('skip for ICU 4.8+');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
//ini_set("intl.default_locale", "nl");

$mf = new MessageFormatter('en_US',
	"{0,number} -- {foo,ordinal}");
	
var_dump($mf->format(array(2.3, "foo" => 1.3)));
var_dump($mf->format(array("foo" => 1.3, 0 => 2.3)));

?>
==DONE==
--EXPECT--
string(10) "2.3 -- 1st"
string(10) "2.3 -- 1st"
==DONE==