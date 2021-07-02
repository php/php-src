--TEST--
Internal enums serialization
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(ZendTestStringBacked::FIRST);
$s = serialize(ZendTestStringBacked::FIRST);
var_dump($s);
var_dump(unserialize($s));
var_dump(unserialize($s) === ZendTestStringBacked::FIRST);

?>
--EXPECT--
enum(ZendTestStringBacked::FIRST)
string(34) "E:26:"ZendTestStringBacked:FIRST";"
enum(ZendTestStringBacked::FIRST)
bool(true)
