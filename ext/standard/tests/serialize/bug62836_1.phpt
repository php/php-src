--TEST--
Bug #62836 (Seg fault or broken object references on unserialize())
--FILE--
<?php
$serialized_object='O:1:"A":4:{s:1:"b";O:1:"B":0:{}s:2:"b1";r:2;s:1:"c";O:1:"B":0:{}s:2:"c1";r:4;}';
spl_autoload_register(function ($name) {
    unserialize("i:4;");
    eval("class $name {} ");
});

print_r(unserialize($serialized_object));
echo "okey";
?>
--EXPECT--
A Object
(
    [b] => B Object
        (
        )

    [b1] => B Object
        (
        )

    [c] => B Object
        (
        )

    [c1] => B Object
        (
        )

)
okey
