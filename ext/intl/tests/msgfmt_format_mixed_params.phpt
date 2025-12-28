--TEST--
MessageFormatter::format(): mixed named and numeric parameters
--EXTENSIONS--
intl
--FILE--
<?php

$mf = new MessageFormatter('en_US',
    "{0,number} -- {foo,ordinal}");

var_dump($mf->format(array(2.3, "foo" => 1.3)));
var_dump($mf->format(array("foo" => 1.3, 0 => 2.3)));

?>
--EXPECT--
string(10) "2.3 -- 1st"
string(10) "2.3 -- 1st"
