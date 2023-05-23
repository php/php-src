--TEST--
Serialization of RC1 objects appearing in object graph twice
--FILE--
<?php

function rcn() {
    $root = new stdClass;
    $end = new stdClass;
    $root->a = [$end];
    $root->b = $root->a;
    unset($end);
    echo serialize($root), "\n";
}

function rcn_rc1() {
    $root = new stdClass;
    $end = new stdClass;
    $root->a = [[$end]];
    $root->b = $root->a;
    unset($end);
    echo serialize($root), "\n";
}

function rcn_properties_ht() {
    $object = new stdClass;
    $object->object = new stdClass;
    $array = (array) $object;
    $root = [$object, $array];
    unset($object);
    unset($array);
    echo serialize($root), "\n";
}

rcn();
rcn_rc1();
rcn_properties_ht();

?>
--EXPECT--
O:8:"stdClass":2:{s:1:"a";a:1:{i:0;O:8:"stdClass":0:{}}s:1:"b";a:1:{i:0;r:3;}}
O:8:"stdClass":2:{s:1:"a";a:1:{i:0;a:1:{i:0;O:8:"stdClass":0:{}}}s:1:"b";a:1:{i:0;a:1:{i:0;r:4;}}}
a:2:{i:0;O:8:"stdClass":1:{s:6:"object";O:8:"stdClass":0:{}}i:1;a:1:{s:6:"object";r:3;}}
