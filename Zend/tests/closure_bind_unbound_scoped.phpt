--TEST--
Closure::bind and ::bindTo unbound_scoped parameter
--FILE--
<?php
class FooBar {
    private $x = 3;
}
$foo = function () {
    var_dump($this->x);
};

// This should create a closure scoped to FooBar but unbound, not static
$foo = $foo->bindTo(NULL, "FooBar", true);

// As it is unbound, not static, this will work
$foo->apply(new FooBar);

$foo = function () {
    var_dump($this->x);
};
$foo = Closure::bind($foo, NULL, "FooBar", true);
$foo->apply(new FooBar);
?>
--EXPECT--
int(3)
int(3)