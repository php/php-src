--TEST--
Check key execution order with new.
--FILE--
<?php
$a[2][3] = 'StdClass';
$a[$i=0][++$i] = new $a[++$i][++$i];
print_r($a);

$o = new StdClass;
$o->a = new $a[$i=2][++$i];
$o->a->b = new $a[$i=2][++$i];
print_r($o);
?>
--EXPECT--
Array
(
    [2] => Array
        (
            [3] => StdClass
        )

    [0] => Array
        (
            [1] => StdClass Object
                (
                )

        )

)
StdClass Object
(
    [a] => StdClass Object
        (
            [b] => StdClass Object
                (
                )

        )

)
