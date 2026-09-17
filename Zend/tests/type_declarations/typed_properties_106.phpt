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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $ary = [1];
    $ref = $ary;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($ref);
?>
--EXPECT--
TypeError: Cannot assign array to reference held by property Test::$prop of type ?Type
TypeError: Cannot assign array to reference held by property Test::$prop of type ?Type
NULL
