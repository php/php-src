--TEST--
Misc behaviors of serializable const-expr closures: static vars, identity, scope binding, invalid reference errors
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL)]
class A {
    public function __construct(public mixed $cb = null) {}
}

class Demo {
    private const SECRET = 'secret';

    #[A(static function () { static $n = 0; return ++$n; })]
    #[A(static function () { return self::SECRET; })]
    public int $p = 0;
}

$attributes = (new ReflectionProperty(Demo::class, 'p'))->getAttributes();
$counter = $attributes[0]->getArguments()[0];
$scoped = $attributes[1]->getArguments()[0];

// A reference does not carry the state of static variables: unserializing
// produces the closure as if it was freshly evaluated.
var_dump($counter(), $counter());
$u = unserialize(serialize($counter));
var_dump($u());

// Unserialized closures are new instances.
var_dump($u === $counter);

// Scope binding is restored: private members of the class are accessible.
var_dump(unserialize(serialize($scoped))());

// Invalid reference errors: resolution happens inside unserialize().
$fromRef = static fn (string $class, string $site, int|string $key) => unserialize('O:7:"Closure":' . substr(serialize([[], ['const-expr', [$class, $site, $key, 0]]]), 2));

foreach ([['NoSuchClass', '$p', 0], ['stdClass', '$p', 0], ['Demo', '$p', 999]] as [$class, $site, $key]) {
    try {
        $fromRef($class, $site, $key);
    } catch (Exception $e) {
        echo get_class($e), ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
int(1)
int(2)
int(1)
bool(false)
string(6) "secret"
Exception: Invalid serialization data for Closure object (cannot load class "NoSuchClass")
Exception: Invalid serialization data for Closure object (cannot load class "stdClass")
Exception: Invalid serialization data for Closure object (constant-expression closure "$p@999" of class Demo not found)
