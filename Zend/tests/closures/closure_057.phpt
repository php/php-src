--TEST--
Closure 057: segfault in leave helper
--FILE--
<?php
class A {
}

function getfunc() {
    $b = function() {
        $a = function() {
        };
        $a();
    };
    return $b->bindTo(new A());
}

call_user_func(getfunc());

echo "okey";
?>
--EXPECT--
okey
