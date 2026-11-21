--TEST--
GH-25380: Stateless closure cache must not retain closures when declaring closure has a heap runtime cache
--FILE--
<?php

class A {}

$outer = function () {
    return (static fn () => static::class)();
};

$gen = function () {
    yield (static fn () => static::class)();
};

trait T {
    public function m($k) {
        $f = function () use ($k) {
            return (static fn ($x) => $x + 1)($k);
        };
        return $f();
    }
}

class U1 { use T; }
class U2 { use T; }

class Repo {
    public function get(array $rows) {
        return array_map(function ($r) {
            return (static fn ($x) => $x * 2)($r);
        }, $rows);
    }
}

$obj = new A();
$u1 = new U1();
$u2 = new U2();
$repo = new Repo();

$cases = [
    'bind' => static function () use ($outer) {
        return Closure::bind($outer, null, A::class)();
    },
    'bindTo' => static function () use ($outer) {
        return $outer->bindTo(null, A::class)();
    },
    'call' => static function () use ($outer, $obj) {
        return $outer->call($obj);
    },
    'generator_bind' => static function () use ($gen) {
        return Closure::bind($gen, null, A::class)()->current();
    },
    'trait_second_class' => static function () use ($u2) {
        return $u2->m(41);
    },
    'method_closure' => static function () use ($repo) {
        return $repo->get([21])[0];
    },
];

// Warm up before measuring
$u1->m(0);
foreach ($cases as $fn) {
    $fn();
}

foreach ($cases as $name => $fn) {
    $before = memory_get_usage();
    for ($i = 0; $i < 20000; $i++) {
        $result = $fn();
    }
    $growth = memory_get_usage() - $before;
    var_dump($name, $result, $growth < 4096);
}

?>
--EXPECT--
string(4) "bind"
string(1) "A"
bool(true)
string(6) "bindTo"
string(1) "A"
bool(true)
string(4) "call"
string(1) "A"
bool(true)
string(14) "generator_bind"
string(1) "A"
bool(true)
string(18) "trait_second_class"
int(42)
bool(true)
string(14) "method_closure"
int(42)
bool(true)
