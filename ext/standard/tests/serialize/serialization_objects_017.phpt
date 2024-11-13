--TEST--
Object serialization / unserialization: Strict format
--FILE--
<?php
class A {public $a;}

var_dump(unserialize('O:1:"A":1x{s:1:"a";N;}'));
                    //0123456789012345678901
var_dump(unserialize('O:1:"A":1:xs:1:"a";N;}'));
                    //0123456789012345678901
?>
--EXPECTF--
Warning: unserialize(): Error at offset 9 of 22 bytes in %s on line %d
bool(false)

Warning: unserialize(): Error at offset 10 of 22 bytes in %s on line %d
bool(false)
