--TEST--
Integer keys in the properties that internal classes load when unserialized
--FILE--
<?php

#[AllowDynamicProperties]
class Storage extends SplObjectStorage {}

$storage = unserialize('O:7:"Storage":2:{i:0;a:0:{}i:1;a:1:{i:7;s:1:"y";}}');
var_dump($storage->{7});
var_dump(clone $storage);

?>
--EXPECT--
string(1) "y"
object(Storage)#2 (2) {
  ["7"]=>
  string(1) "y"
  ["storage":"SplObjectStorage":private]=>
  array(0) {
  }
}
