--TEST--
Bug #71837 (Wrong arrays behaviour)
--FILE--
<?php

$p = array(array());
array_push($p[0], array(100));

$c = array_merge($p, array());
$c[0][0] = 200;

print_r($p);

?>
--EXPECT--
Array
(
    [0] => Array
        (
            [0] => Array
                (
                    [0] => 100
                )

        )

)
