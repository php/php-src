--TEST--
Lazy Objects: resetAsLazy() during foreach() is not allowed
--FILE--
<?php

class NoHooks {
    public int $a = 1;
}

class Hooks {
    public int $a = 1;
    public int $b { get { return 2; } }
}

function test(string $name, object $obj) {
    printf("# %s\n", $name);
    $reflector = new ReflectionClass($obj::class);
    foreach ($obj as $key => $value) {
        var_dump($key);
        try {
            $reflector->resetAsLazyProxy($obj, function () {});
        } catch (Error $e) {
            printf("%s: %s\n", $e::class, $e->getMessage());
        }
    }
}

$nohooks = new ReflectionClass(NoHooks::class);
$hooks = new ReflectionClass(Hooks::class);

$obj = $nohooks->newLazyGhost(function () {});

test('Ghost', $obj);

$obj = $hooks->newLazyGhost(function () {});

test('Ghost (hooks)', $obj);

$obj = $nohooks->newLazyProxy(function () {
    return new NoHooks();
});

test('Proxy', $obj);

$obj = $hooks->newLazyProxy(function () {
    return new Hooks();
});

test('Proxy', $obj);

$obj = new NoHooks();

test('Non lazy', $obj);

$obj = new Hooks();

test('Non lazy (hooks)', $obj);

$obj = new NoHooks();
ob_start();
var_dump($obj);
ob_end_clean();

test('Non lazy with ht', $obj);

$obj = new Hooks();
ob_start();
var_dump($obj);
ob_end_clean();

test('Non lazy with ht (hooks)', $obj);

?>
==DONE==
--EXPECT--
# Ghost
string(1) "a"
Error: Can not reset an object during property iteration
# Ghost (hooks)
string(1) "a"
Error: Can not reset an object during property iteration
string(1) "b"
Error: Can not reset an object during property iteration
# Proxy
string(1) "a"
Error: Can not reset an object during property iteration
# Proxy
string(1) "a"
Error: Can not reset an object during property iteration
string(1) "b"
Error: Can not reset an object during property iteration
# Non lazy
string(1) "a"
Error: Can not reset an object during property iteration
# Non lazy (hooks)
string(1) "a"
Error: Can not reset an object during property iteration
string(1) "b"
Error: Can not reset an object during property iteration
# Non lazy with ht
string(1) "a"
Error: Can not reset an object during property iteration
# Non lazy with ht (hooks)
string(1) "a"
Error: Can not reset an object during property iteration
string(1) "b"
Error: Can not reset an object during property iteration
==DONE==
