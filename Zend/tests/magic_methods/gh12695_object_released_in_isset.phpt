--TEST--
GH-12695: Object freed by __isset() during materialization
--FILE--
<?php

/* The re-check after __isset() copies the materialised value into the
 * caller's return-value buffer before releasing the object. This is
 * required because __isset() may drop the last external reference to
 * the object (here via $obj = null), so the property table is freed
 * by OBJ_RELEASE() right after the re-check. */

class C {
    public $prop;
    public function __isset($name) {
        global $obj;
        $obj = null;
        $this->prop = 'materialised';
        return true;
    }
}

$obj = new C();
unset($obj->prop);
var_dump($obj->prop ?? 'fb');

?>
--EXPECT--
string(12) "materialised"
