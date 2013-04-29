--TEST--
Testing nested array shortcut
--FILE--
<?php
print_r([1, 2, 3, ["foo" => "orange", "bar" => "apple", "baz" => "lemon"]]);
?>
--EXPECT--
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
    [3] => Array
        (
            [foo] => orange
            [bar] => apple
            [baz] => lemon
        )

)
