--TEST--
Bug #73154: serialize object with __sleep function crash
--FILE--
<?php
class a {
    public $a;
    public function __sleep() {
        $this->a=null;
        return array();
    }
}
$s = 'a:1:{i:0;O:1:"a":1:{s:1:"a";R:2;}}';
var_dump(serialize(unserialize($s)));
?>
--EXPECTF--
Deprecated: The __sleep() serialization magic method has been deprecated. Implement __serialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
string(22) "a:1:{i:0;O:1:"a":0:{}}"
