--TEST--
GH-17736 (Assertion failure zend_reference_destroy())
--EXTENSIONS--
simplexml
--FILE--
<?php
$o1 = new SimpleXMLElement('<a/>');
class C {
    public int $a = 1;
}
function test($obj) {
    $ref =& $obj->a;
}
$obj = new C;
test($obj);
test($o1);
echo "Done\n";
?>
--EXPECT--
Done
