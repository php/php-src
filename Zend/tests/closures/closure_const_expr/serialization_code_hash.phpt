--TEST--
Anonymous const-expr closure references embed a code hash in their id
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

$id = (new ReflectionFunction(
    (new ReflectionProperty(Ordered::class, 'p'))->getAttributes()[0]->getArguments()[0]
))->getConstExprId();

// The id is "<site>@<rank>#<hash>"; split it to retarget the reference.
var_dump(preg_match('/^\$p@0#[0-9a-f]{8}$/', $id));
$hash = substr($id, strpos($id, '#'));

$ref = static fn (string $class, string $id): string =>
    'O:7:"Closure":' . substr(serialize([[], ['const-expr', [$class, $id]]]), 2);

// Same class: resolves, and fromConstExpr() verifies the same way.
var_dump(unserialize($ref('Ordered', $id))());
var_dump(Closure::fromConstExpr('Ordered', $id)());

// The swapped class has B at rank 0 of $q; same rank, but the hash guards it.
try {
    unserialize($ref('Swapped', '$q@0' . $hash));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
try {
    Closure::fromConstExpr('Swapped', '$q@0' . $hash);
} catch (ValueError $e) {
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
var_dump(unserialize($ref('Reformatted', '$p@0' . $hash))());

// ...but not against a changed body.
class Changed {
    #[A(static function () { return 'CHANGED'; })]
    public int $p = 0;
}
try {
    unserialize($ref('Changed', '$p@0' . $hash));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// An id without a hash (a producer that cannot compute it) resolves
// positionally, unverified.
var_dump(unserialize($ref('Ordered', '$p@0'))());

?>
--EXPECTF--
int(1)
string(1) "a"
string(1) "a"
Invalid serialization data for Closure object (constant-expression closure "$q@0#%s" of class Swapped changed)
Closure::fromConstExpr(): Argument #2 ($id) refers to a constant-expression closure of class Swapped that has changed
string(1) "a"
Invalid serialization data for Closure object (constant-expression closure "$p@0#%s" of class Changed changed)
string(1) "a"
