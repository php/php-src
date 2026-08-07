--TEST--
Test array promotion does not violate type restrictions
--FILE--
<?php

class Foo {
    public ?string $p;
    public ?iterable $i;
    public static ?string $s;
    public static ?array $a;
}

$a = new Foo;

$a->i[] = 1;
var_dump($a->i);

try {
    $a->p[] = "test";
} catch (TypeError $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { // must be uninit
    var_dump($a->p); // WRONG!
} catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

$a->p = null;
try {
    $a->p[] = "test";
} catch (TypeError $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump($a->p);

Foo::$a["bar"] = 2;
var_dump(Foo::$a);

try {
    Foo::$s["baz"][] = "baz";
} catch (TypeError $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
try { // must be uninit
    var_dump(Foo::$s);
} catch (Error $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }

Foo::$a = null;
$ref = &Foo::$a;
$ref[] = 3;
var_dump($ref);

$ref = &$a->p;
try {
    $ref[] = "bar";
} catch (TypeError $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump($ref);

try {
    $ref["baz"][] = "bar"; // indirect assign
} catch (TypeError $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump($ref);

?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
TypeError: Cannot auto-initialize an array inside property Foo::$p of type ?string
Error: Typed property Foo::$p must not be accessed before initialization
TypeError: Cannot auto-initialize an array inside property Foo::$p of type ?string
NULL
array(1) {
  ["bar"]=>
  int(2)
}
TypeError: Cannot auto-initialize an array inside property Foo::$s of type ?string
Error: Typed static property Foo::$s must not be accessed before initialization
array(1) {
  [0]=>
  int(3)
}
TypeError: Cannot auto-initialize an array inside a reference held by property Foo::$p of type ?string
NULL
TypeError: Cannot auto-initialize an array inside a reference held by property Foo::$p of type ?string
NULL
