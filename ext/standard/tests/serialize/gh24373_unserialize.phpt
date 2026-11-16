--TEST--
GH-24373 (unserialize() called from a destructor during var_hash destruction reuses the outer var_hash)
--FILE--
<?php
class D {
    public function __destruct() {
        $GLOBALS['r'] = unserialize('a:2:{i:0;O:8:"stdClass":0:{}i:1;r:2;}');
    }
}
unserialize('a:2:{i:0;O:1:"D":0:{}i:0;i:1;}');
var_dump($GLOBALS['r'][0] === $GLOBALS['r'][1]);
?>
--EXPECT--
bool(true)
