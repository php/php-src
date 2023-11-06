--TEST--
Type inference verification 001
--FILE--
<?php
function escape($val) {
    global $g;
    $g = $val;
}
function test() {
    $val = date_create();
    escape($val);
    spl_object_id($val);
}
test();
?>
===DONE===
--EXPECT--
===DONE===
