--TEST--
__sleep() returns properties clashing only after mangling
--FILE--
<?php
class Test {
    private $priv;
    public function __sleep() {
        return ["\0Test\0priv", "priv"];
    }
}
$s = serialize(new Test);
var_dump(str_replace("\0", '\0', $s));
?>
--EXPECTF--
Warning: serialize(): "priv" is returned from __sleep() multiple times in %s on line %d
string(37) "O:4:"Test":1:{s:10:"\0Test\0priv";N;}"
