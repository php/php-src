--TEST--
Type inference verification 003
--FILE--
<?php
function test() {
    $val = date_create();
    [$val, spl_object_id($val)];
}
test();
?>
===DONE===
--EXPECT--
===DONE===
