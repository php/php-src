--TEST--
http_build_query() function
--FILE--
<?php

$array = array("foo"=>"bar","baz"=>1,"test"=>"a ' \" ", "abc", 'float' => 10.42, 'true' => true, 'false' => false);
var_dump(http_build_query($array));
var_dump(http_build_query($array, 'foo'));
var_dump(http_build_query($array, 'foo', ';'));

?>
--EXPECT--
string(62) "foo=bar&baz=1&test=a+%27+%22+&0=abc&float=10.42&true=1&false=0"
string(65) "foo=bar&baz=1&test=a+%27+%22+&foo0=abc&float=10.42&true=1&false=0"
string(65) "foo=bar;baz=1;test=a+%27+%22+;foo0=abc;float=10.42;true=1;false=0"
