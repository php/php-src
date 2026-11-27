--TEST--
Test Uri\QueryParams::sortByUtf16() - embedded NUL, prefixes and stable duplicate names
--FILE--
<?php

$params1 = (new Uri\QueryParams())
    ->append("a\0b", "z")
    ->append("a\0a", "2")
    ->append("a\0b", "a")
    ->append("a\0", "3")
    ->append("a", "4")
    ->append("", "5");
$params2 = $params1->sortByUtf16();

var_dump($params1->toRfc3986String());
var_dump($params2->toRfc3986String());
var_dump($params2->getAll("a\0b"));

?>
--EXPECT--
string(37) "a%00b=z&a%00a=2&a%00b=a&a%00=3&a=4&=5"
string(37) "=5&a=4&a%00=3&a%00a=2&a%00b=z&a%00b=a"
array(2) {
  [0]=>
  string(1) "z"
  [1]=>
  string(1) "a"
}
