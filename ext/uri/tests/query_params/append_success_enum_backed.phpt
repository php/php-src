--TEST--
Test Uri\QueryParams::append() - success - backed enum
--FILE--
<?php

enum StringEnum: string {
    case Bar = "bar";
}

enum IntEnum: int {
    case Baz = 123;
}

$params = Uri\QueryParams::parseRfc3986("");
$params = $params->append("foo", StringEnum::Bar);
$params = $params->append("baz", IntEnum::Baz);

var_dump($params->toList());
var_dump($params->toRfc1866String());
var_dump($params->toRfc3986String());
var_dump($params->toWhatWgUrlString());

?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(3) "foo"
    [1]=>
    string(3) "bar"
  }
  [1]=>
  array(2) {
    [0]=>
    string(3) "baz"
    [1]=>
    string(3) "123"
  }
}
string(15) "foo=bar&baz=123"
string(15) "foo=bar&baz=123"
string(15) "foo=bar&baz=123"
