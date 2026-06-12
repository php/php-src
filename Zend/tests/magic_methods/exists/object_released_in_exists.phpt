--TEST--
__exists: object freed by __exists() during materialization (??, isset, empty)
--FILE--
<?php

/* When __exists() materialises the property and the object's last
 * external reference is dropped during the call, the property table
 * is freed by the OBJ_RELEASE() after the re-check.
 *
 * - For `??` (read_property): the materialised value is copied into the
 *   caller's `rv` buffer before OBJ_RELEASE, otherwise the returned
 *   pointer dangles.
 * - For isset()/empty() (has_property): the property's value is read
 *   into a local bool before OBJ_RELEASE, so there is no dangling
 *   pointer to return; this test pins that down. */

class C {
    public $prop;
    public function __exists(string $name): bool {
        global $obj;
        $obj = null;
        $this->prop = 'materialised';
        return true;
    }
    public function __get(string $n): mixed {
        throw new Exception("__get must not be called when __exists materialised the property");
    }
}

echo "1) `??`:\n";
$obj = new C();
unset($obj->prop);
var_dump($obj->prop ?? 'fb');

echo "\n2) isset():\n";
$obj = new C();
unset($obj->prop);
var_dump(isset($obj->prop));

echo "\n3) empty():\n";
$obj = new C();
unset($obj->prop);
var_dump(empty($obj->prop));

?>
--EXPECT--
1) `??`:
string(12) "materialised"

2) isset():
bool(true)

3) empty():
bool(false)
