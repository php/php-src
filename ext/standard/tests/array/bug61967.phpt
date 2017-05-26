--TEST--
Bug #61967: unset array item in array_walk_recursive cause inconsistent array
--FILE--
<?php
$arr = array(
    range(1, 5),
    range(1, 5),
    range(1, 5),
    range(1, 5),
    range(1, 5),
);

array_walk_recursive($arr,
    function (&$value, $key) use(&$arr) {
        var_dump($key);
        unset($arr[$key]);
    }
);
?>
--EXPECT--
int(0)
int(1)
int(2)
int(3)
int(4)
