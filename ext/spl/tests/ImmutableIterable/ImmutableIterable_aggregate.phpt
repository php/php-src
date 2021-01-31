--TEST--
ImmutableIterable is an IteratorAggregate
--FILE--
<?php

$it = new ImmutableIterable(['first' => 'x', 'second' => (object)['key' => 'value']]);
foreach ($it as $k1 => $v1) {
    foreach ($it as $k2 => $v2) {
        printf("k1=%s k2=%s v1=%s v2=%s\n", json_encode($k1), json_encode($k2), json_encode($v1), json_encode($v2));
    }
}
?>
--EXPECT--
k1="first" k2="first" v1="x" v2="x"
k1="first" k2="second" v1="x" v2={"key":"value"}
k1="second" k2="first" v1={"key":"value"} v2="x"
k1="second" k2="second" v1={"key":"value"} v2={"key":"value"}