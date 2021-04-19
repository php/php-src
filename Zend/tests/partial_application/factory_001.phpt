--TEST--
Partial application factory: pass
--FILE--
<?php
class Foo {
    public function __destruct() {
        printf("%s\n", __METHOD__);
    }
}

$foo = new Foo(...);

$two = [$foo(), $foo()];

if ($two[0] === $two[1]) {
    var_dump($two);
} else {
    echo "OK\n";
}
?>
--EXPECTF--
OK
Foo::__destruct
Foo::__destruct
