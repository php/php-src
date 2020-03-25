--TEST--
Bug #73052: Memory Corruption in During Deserialized-object Destruction
--FILE--
<?php

class obj {
    var $ryat;
    public function __destruct() {
        $this->ryat = null;
    }
}

$poc = 'O:3:"obj":1:{';
var_dump(unserialize($poc));
?>
--EXPECTF--
Notice: unserialize(): Error at offset 13 of 13 bytes in %sbug73052.php on line %d
bool(false)
