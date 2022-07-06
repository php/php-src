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
--EXPECT--
string(22) "a:1:{i:0;O:1:"a":0:{}}"
