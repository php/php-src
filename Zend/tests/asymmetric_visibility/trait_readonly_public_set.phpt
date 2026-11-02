--TEST--
public(set) of trait property is properly propagated
--FILE--
<?php

trait T {
    public public(set) readonly int $prop;
}

class C {
    use T;
}

$c = new C;
$c->prop = 42;

try {
    $c->prop = 43;
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($c->prop);

?>
--EXPECT--
Error: Cannot modify readonly property C::$prop
int(42)
