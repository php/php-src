--TEST--
Anonymous const-expr closure references carry a code hash
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL)]
class A {
    public function __construct(public mixed $cb = null) {}
}
#[Attribute(Attribute::TARGET_ALL)]
class B {
    public function __construct(public mixed $cb = null) {}
}

// Serialize the closure of attribute A (rank 0 of the property), then resolve
// the same reference against a class where A and B are swapped: rank 0 now
// names B's closure, but the code hash embedded in the id no longer matches,
// so it fails loudly instead of silently returning B's closure.
class Ordered {
    #[A(static function () { return 'a'; })]
    #[B(static function () { return 'b'; })]
    public int $p = 0;
}
class Swapped {
    #[B(static function () { return 'b'; })]
    #[A(static function () { return 'a'; })]
    public int $q = 0;
}

$ref = (new ReflectionProperty(Ordered::class, 'p'))->getAttributes()[0]->getArguments()[0]
    ->__serialize()[1][1];

// The reference is [class, site, rank, hash]: rank 0 of $p, with a non-zero
// code hash in the fourth field.
var_dump($ref[1] === '$p' && $ref[2] === 0 && is_int($ref[3]) && $ref[3] !== 0);
$hash = $ref[3];

$make = static fn (string $class, string $site, int|string $key, int $hash): string =>
    'O:7:"Closure":' . substr(serialize([[], ['const-expr', [$class, $site, $key, $hash]]]), 2);

// Same class: resolves.
var_dump(unserialize($make('Ordered', '$p', 0, $hash))());

// The swapped class has B at rank 0 of $q; same rank, but the hash guards it.
try {
    unserialize($make('Swapped', '$q', 0, $hash));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// The hash is over the closure's code, insensitive to layout and comments: a
// reference resolves against a reformatted body...
class Reformatted {
    #[A(static function () {
        // a comment; different whitespace and layout
        return    'a' ;
    })]
    public int $p = 0;
}
var_dump(unserialize($make('Reformatted', '$p', 0, $hash))());

// ...but not against a changed body.
class Changed {
    #[A(static function () { return 'CHANGED'; })]
    public int $p = 0;
}
try {
    unserialize($make('Changed', '$p', 0, $hash));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// A reference without a hash (a producer that cannot compute it) resolves
// positionally, unverified.
var_dump(unserialize($make('Ordered', '$p', 0, 0))());

?>
--EXPECT--
bool(true)
string(1) "a"
Invalid serialization data for Closure object (constant-expression closure "$q@0" of class Swapped changed)
string(1) "a"
Invalid serialization data for Closure object (constant-expression closure "$p@0" of class Changed changed)
string(1) "a"
