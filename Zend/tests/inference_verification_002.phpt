--TEST--
Type inference verification 002
--FILE--
<?php
function test() {
    $val = date_create();
    $alias = $val;
    spl_object_id($val);
}
test();
?>
===DONE===
--EXPECT--
===DONE===
