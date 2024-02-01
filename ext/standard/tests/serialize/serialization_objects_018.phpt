--TEST--
Object serialization / unserialization: Strict format (2)
--FILE--
<?php
class A implements Serializable {
	public function serialize() {}
	public function unserialize($data) {}
	public function __serialize() {}
	public function __unserialize($data) {}
}

var_dump(unserialize('C:1:"A":3x{foo}'));
                    //012345678901234
var_dump(unserialize('C:1:"A":3:xfoo}'));
                    //012345678901234
var_dump(unserialize('C:1:"A":3:{foox'));
                    //012345678901234
var_dump(unserialize('C:1:"A":'));
                    //01234567

?>
--EXPECTF--
Notice: unserialize(): Error at offset 9 of 15 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 10 of 15 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 14 of 15 bytes in %s on line %d
bool(false)

Notice: unserialize(): Error at offset 8 of 8 bytes in %s on line %d
bool(false)
