--TEST--
Collections\Deque is an IteratorAggregate
--FILE--
<?php

$it = new Collections\Deque(['discarded_first' => 'x', 'discardedsecond' => (object)['key' => 'value']]);
foreach ($it as $k1 => $v1) {
    foreach ($it as $k2 => $v2) {
        printf("k1=%s k2=%s v1=%s v2=%s\n", json_encode($k1), json_encode($k2), json_encode($v1), json_encode($v2));
    }
}
?>
--EXPECT--
k1=0 k2=0 v1="x" v2="x"
k1=0 k2=1 v1="x" v2={"key":"value"}
k1=1 k2=0 v1={"key":"value"} v2="x"
k1=1 k2=1 v1={"key":"value"} v2={"key":"value"}