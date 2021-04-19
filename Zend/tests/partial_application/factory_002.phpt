--TEST--
Partial application factory: normal
--FILE--
<?php
class Foo {
    public function __construct($a) {
        printf("%s\n", __METHOD__);
    }

    public function __destruct() {
        printf("%s\n", __METHOD__);
    }
}

$foo = new Foo(...);

$two = [$foo(1), $foo(1)];

if ($two[0] === $two[1]) {
    var_dump($two);
} else {
    echo "OK\n";
}
?>
--EXPECTF--
Foo::__construct
Foo::__construct
OK
Foo::__destruct
Foo::__destruct
