--TEST--
MessageFormatter::format(): mixed named and numeric parameters
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
//ini_set("intl.default_locale", "nl");

$mf = new MessageFormatter('en_US',
    "{0,number} -- {foo,ordinal}");

var_dump($mf->format(array(2.3, "foo" => 1.3)));
var_dump($mf->format(array("foo" => 1.3, 0 => 2.3)));

?>
--EXPECT--
string(10) "2.3 -- 1st"
string(10) "2.3 -- 1st"
