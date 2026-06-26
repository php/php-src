--TEST--
Turbofish arity: ReflectionAttribute::newInstance() enforces arity against the attribute class
--FILE--
<?php
#[Attribute]
class TaggedAttr<T> {
    public function __construct(public string $tag) {}
}

#[TaggedAttr::<int>(tag: "ok")]
class A {}

#[TaggedAttr::<int, string>(tag: "bad")]
class B {}

#[TaggedAttr(tag: "no-turbo")]
class D {}

$ra = (new ReflectionClass("A"))->getAttributes()[0];
echo "A: ", $ra->newInstance()->tag, "\n";

$rb = (new ReflectionClass("B"))->getAttributes()[0];
try {
    $rb->newInstance();
} catch (ArgumentCountError $e) {
    echo "B: ", $e->getMessage(), "\n";
}

// no turbofish at all -> no arity check, defaults apply (here T has no default)
// but newInstance() doesn't supply type args, which is arity 0; required is 1 -> still fails? No.
// arity 0 is treated as "no turbofish" and skips the check entirely.
$rd = (new ReflectionClass("D"))->getAttributes()[0];
echo "D: ", $rd->newInstance()->tag, "\n";
?>
--EXPECT--
A: ok
B: Too many generic type arguments to new TaggedAttr, 2 passed and exactly 1 expected
D: no-turbo
