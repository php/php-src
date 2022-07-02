--TEST--
Bug #46215 (json_encode mutates its parameter and has some class-specific state)
--FILE--
<?php

class foo {
    protected $a = array();
}

$a = new foo;
$x = json_encode($a);

print_r($a);

?>
--EXPECT--
foo Object
(
    [a:protected] => Array
        (
        )

)
