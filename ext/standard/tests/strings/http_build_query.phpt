--TEST--
http_build_query() function
--FILE--
<?php

$array = array("foo"=>"bar","baz"=>1,"test"=>"a ' \" ", "abc");
var_dump(http_build_query($array));
var_dump(http_build_query($array, 'foo'));
var_dump(http_build_query($array, 'foo', ';'));

?>
--EXPECT--
string(35) "foo=bar&baz=1&test=a+%27+%22+&0=abc"
string(38) "foo=bar&baz=1&test=a+%27+%22+&foo0=abc"
string(38) "foo=bar;baz=1;test=a+%27+%22+;foo0=abc"
