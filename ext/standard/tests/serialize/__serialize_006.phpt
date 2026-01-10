--TEST--
Failure while parsing data array for __unserialize()
--FILE--
<?php

class Test {
    public function __unserialize(array $data) { }
}

var_dump(unserialize('O:4:"Test":1:{}'));

?>
--EXPECTF--
Warning: unserialize(): Unexpected end of serialized data in %s on line %d

Warning: unserialize(): Error at offset 14 of 15 bytes in %s on line %d
bool(false)
