--TEST--
CONST/CV should not be freed on failed reference assignment
--FILE--
<?php
class Test {
    public ?Type $prop;
}
$obj = new Test;
$ref =& $obj->prop;
try {
    $ref = [1];
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $ary = [1];
    $ref = $ary;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($ref);
?>
--EXPECT--
Cannot assign array to reference held by property Test::$prop of type ?Type
Cannot assign array to reference held by property Test::$prop of type ?Type
NULL
