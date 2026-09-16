--TEST--
Test print_r() function with circular array
--FILE--
<?php

$a1 = [];
$a2 = [&$a1];
$a1[] =& $a2;

print_r($a2);

?>
--EXPECT--
Array
(
    [0] => Array
        (
            [0] => Array
 *RECURSION*
        )

)
