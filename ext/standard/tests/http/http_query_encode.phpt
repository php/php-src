--TEST--
http_query_encode() function
--FILE--
<?php

$array = array("foo"=>"bar","baz"=>1,"test"=>"a ' \" ", "abc");
var_dump(http_query_encode($array));
var_dump(http_query_encode($array, [
    'numeric_prefix' => 'foo'
]));
var_dump(http_query_encode($array, [
    'numeric_prefix' => 'foo',
    'arg_separator' => ';'
]));

?>
--EXPECT--
string(35) "foo=bar&baz=1&test=a+%27+%22+&0=abc"
string(38) "foo=bar&baz=1&test=a+%27+%22+&foo0=abc"
string(38) "foo=bar;baz=1;test=a+%27+%22+;foo0=abc"
