--TEST--
Bug #80096: Segmentation fault with named arguments in nested call
--FILE--
<?php

function println($arg) {
    echo $arg, "\n";
}

println(htmlentities("The < character is encoded as &lt;", double_encode: false));

?>
--EXPECT--
The &lt; character is encoded as &lt;
