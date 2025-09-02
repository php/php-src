--TEST--
Foreach by ref assignment to property
--FILE--
<?php
$obj = new stdClass;
foreach ([0] as &$obj->prop) {
    var_dump($obj->prop);
}
?>
--EXPECT--
int(0)
