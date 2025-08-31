--TEST--
Closure comparison
--FILE--
<?php
function foo() {
    static $var;
}

$closures[0] = Closure::fromCallable('foo');
$closures[1] = Closure::fromCallable('foo');

printf("foo == foo: %s\n", $closures[0] == $closures[1] ? "OK" : "FAIL");

$closures[0] = Closure::fromCallable('strlen');
$closures[1] = Closure::fromCallable('strlen');

printf("strlen == strlen: %s\n", $closures[0] == $closures[1] ? "OK" : "FAIL");

$closures[0] = Closure::fromCallable('strlen');
$closures[1] = Closure::fromCallable('strrev');

printf("strlen != strrev: %s\n", $closures[0] != $closures[1] ? "OK" : "FAIL");

trait MethodTrait {
    public function traitMethod(){}
}

class Foo {
    use MethodTrait {
        MethodTrait::traitMethod as aliasMethod;
    }

    public function __call($method, $args) {
    
    }

    public function exists() {}

    public static function existsStatic() {}
}

class Bar extends Foo {}

class Baz {
    use MethodTrait;
}

$closures[0] = Closure::fromCallable([Foo::class, "existsStatic"]);
$closures[1] = Closure::fromCallable([Bar::class, "existsStatic"]);

printf("foo::existsStatic != bar::existsStatic: %s\n", $closures[0] != $closures[1] ? "OK" : "FAIL");

$foo = new Foo;

$closures[0] = Closure::fromCallable([$foo, "exists"]);
$closures[1] = $closures[0]->bindTo(new Foo);

printf("foo#0::exists != foo#1::exists: %s\n", $closures[0] != $closures[1] ? "OK" : "FAIL");

$baz = new Baz;

$closures[0] = Closure::fromCallable([$foo, "traitMethod"]);
$closures[1] = Closure::fromCallable([$baz, "traitMethod"]);

printf("foo::traitMethod != baz::traitMethod: %s\n", $closures[0] != $closures[1] ? "OK" : "FAIL");

$closures[0] = Closure::fromCallable([$foo, "traitMethod"]);
$closures[1] = Closure::fromCallable([$foo, "aliasMethod"]);

printf("foo::traitMethod != foo::aliasMethod: %s\n", $closures[0] != $closures[1] ? "OK" : "FAIL");

$closures[0] = Closure::fromCallable([$foo, "exists"]);
$closures[1] = Closure::fromCallable([$foo, "exists"]);

printf("foo::exists == foo::exists: %s\n", $closures[0] == $closures[1] ? "OK" : "FAIL");

$closures[0] = Closure::fromCallable([$foo, "method"]);
$closures[1] = Closure::fromCallable([$foo, "method"]);

printf("foo::method == foo::method: %s\n", $closures[0] == $closures[1] ? "OK" : "FAIL");

$closures[1] = $closures[1]->bindTo(new Bar);

printf("foo::method != bar::method: %s\n", $closures[0] != $closures[1] ? "OK" : "FAIL");

$closures[0] = Closure::fromCallable([$foo, "method"]);
$closures[1] = Closure::fromCallable([$foo, "method2"]);

printf("foo::method != foo::method2: %s\n", $closures[0] != $closures[1] ? "OK" : "FAIL");

$closures[2] = Closure::fromCallable([$closures[0], "__invoke"]);
$closures[3] = Closure::fromCallable([$closures[1], "__invoke"]);

printf("Closure[0]::invoke != Closure[1]::invoke: %s\n", $closures[2] != $closures[3] ? "OK" : "FAIL");

$closures[2] = Closure::fromCallable([$closures[0], "__invoke"]);
$closures[3] = Closure::fromCallable([$closures[0], "__invoke"]);

printf("Closure[0]::invoke == Closure[0]::invoke: %s\n", $closures[2] == $closures[3] ? "OK" : "FAIL");
?>
--EXPECT--
foo == foo: OK
strlen == strlen: OK
strlen != strrev: OK
foo::existsStatic != bar::existsStatic: OK
foo#0::exists != foo#1::exists: OK
foo::traitMethod != baz::traitMethod: OK
foo::traitMethod != foo::aliasMethod: OK
foo::exists == foo::exists: OK
foo::method == foo::method: OK
foo::method != bar::method: OK
foo::method != foo::method2: OK
Closure[0]::invoke != Closure[1]::invoke: OK
Closure[0]::invoke == Closure[0]::invoke: OK
