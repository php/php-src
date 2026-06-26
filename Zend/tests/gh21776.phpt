--TEST--
GH-21776 (Heap use-after-free in zend_object_is_lazy via magic __isset)
--FILE--
<?php
class C {
    function __isset($x) {
        $GLOBALS['o'] = 0;
        return true;
    }
}
$o = new C;
$o->a ?? 0;
echo "OK\n";
?>
--EXPECT--
OK
