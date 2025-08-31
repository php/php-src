--TEST--
GH-17162 (zend_array_try_init() with dtor can cause engine UAF)
--FILE--
<?php
class Test {
    function __destruct() {
        global $box;
        $box->value = null;
    }
}
$box = [new Test];
// Using getimagesize() for the test because it's always available,
// but any function that uses zend_try_array_init() would work.
try {
    getimagesize("dummy", $box);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Attempt to assign property "value" on null
