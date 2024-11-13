--TEST--
GH-14652 segfault on object cloning
--EXTENSIONS--
dom
--CREDITS--
YuanchengJiang
--FILE--
<?php
$attr = new DOMAttr('category', 'books');
$clone = clone $attr;
$attr->value = "hello";

var_dump($attr->value);
var_dump($clone->value);
?>
--EXPECT--
string(5) "hello"
string(5) "books"
