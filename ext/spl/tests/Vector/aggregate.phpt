--TEST--
Vector is an IteratorAggregate
--FILE--
<?php

$vector = new Vector(['x', (object)['key' => 'value']]);
foreach ($vector as $k => $v) {
    foreach ($vector as $k2 => $v2) {
        printf("k=%s k2=%s v=%s v2=%s\n", json_encode($k), json_encode($k2), json_encode($v), json_encode($v2));
    }
}
?>
--EXPECT--
k=0 k2=0 v="x" v2="x"
k=0 k2=1 v="x" v2={"key":"value"}
k=1 k2=0 v={"key":"value"} v2="x"
k=1 k2=1 v={"key":"value"} v2={"key":"value"}