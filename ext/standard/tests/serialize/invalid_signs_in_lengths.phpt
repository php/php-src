--TEST--
Lengths and references are not signed
--FILE--
<?php

var_dump(unserialize('s:+1:"x";'));
var_dump(unserialize('s:-1:"x";'));
var_dump(unserialize('a:+0:{}'));
var_dump(unserialize('a:-0:{}'));
var_dump(unserialize('O:+8:"stdClass":0:{}'));
var_dump(unserialize('O:-8:"stdClass":0:{}'));
var_dump(unserialize('C:+11:"ArrayObject":0:{}'));
var_dump(unserialize('C:-11:"ArrayObject":0:{}'));
var_dump(unserialize('a:1:{i:0;r:+1;}'));
var_dump(unserialize('a:1:{i:0;r:-1;}'));
var_dump(unserialize('a:1:{i:0;R:+1;}'));
var_dump(unserialize('a:1:{i:0;R:-1;}'));

?>
--EXPECTF--
Notice: unserialize(): Error at offset 0 of 9 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 0 of 9 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 0 of 7 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 0 of 7 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 0 of 20 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 0 of 20 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 0 of 24 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 0 of 24 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 9 of 15 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 9 of 15 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 9 of 15 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 9 of 15 bytes in %s on line %d
bool(false)
