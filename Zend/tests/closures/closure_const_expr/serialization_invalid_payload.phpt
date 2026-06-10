--TEST--
Unserializing invalid or stale Closure declaration-site references
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL)]
class A {
    public function __construct(public mixed $cb = null) {}
}

class Demo {
    #[A(static function () { return 'ok'; })]
    public int $p = 0;
}

$closure = (new ReflectionProperty(Demo::class, 'p'))->getAttributes()[0]->getArguments()[0];
// The reference is [class, site, key, hash]. For an anonymous closure the key
// is an int rank and hash verifies its code; a producer that cannot compute the
// hash may pass 0, and resolution is then positional.
$hash = $closure->__serialize()[1][1][3];
$stale = ($hash ^ 1) ?: 2;

// Serialize an array and rebrand it as a Closure object payload, so we can
// feed __unserialize() arbitrary tagged-union shapes.
$obj = static fn (array $data): string => 'O:7:"Closure":' . substr(serialize($data), 2);

// Tagged-union payload: [ [], ["const-expr", [class, site, key, hash]] ].
$mk = static fn (string $class, string $site, int|string $key, int $hash): string => $obj([[], ['const-expr', [$class, $site, $key, $hash]]]);

// Sanity checks: a full reference works, and so does a hash-less one.
var_dump(unserialize($mk('Demo', '$p', 0, $hash))());
var_dump(unserialize($mk('Demo', '$p', 0, 0))());

$payloads = [
    'empty data' => $obj([]),
    'one element' => $obj([[]]),
    'unknown tag' => $obj([[], ['whatever', ['Demo', '$p', 0, $hash]]]),
    'tag not list' => $obj([[], 'const-expr']),
    'short reference' => $obj([[], ['const-expr', ['Demo', '$p', 0]]]),
    'wrong key type' => $obj([[], ['const-expr', ['Demo', '$p', [], 0]]]),
    'wrong hash type' => $obj([[], ['const-expr', ['Demo', '$p', 0, '0']]]),
    'unknown class' => $mk('NoSuchClass', '$p', 0, $hash),
    'internal class' => $mk('stdClass', '$p', 0, $hash),
    'unknown site' => $mk('Demo', '$nope', 0, 0),
    'unknown rank' => $mk('Demo', '$p', 9, 0),
    'stale hash' => $mk('Demo', '$p', 0, $stale),
];

foreach ($payloads as $name => $payload) {
    try {
        unserialize($payload);
        echo "$name: unserialized!?\n";
    } catch (Exception $e) {
        echo "$name: {$e->getMessage()}\n";
    }
}

// __unserialize() cannot be used to reinitialize a live closure.
try {
    $closure->__unserialize([[], ['const-expr', ['Demo', '$p', 0, $hash]]]);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(2) "ok"
string(2) "ok"
empty data: Invalid serialization data for Closure object
one element: Invalid serialization data for Closure object
unknown tag: Invalid serialization data for Closure object
tag not list: Invalid serialization data for Closure object
short reference: Invalid serialization data for Closure object
wrong key type: Invalid serialization data for Closure object
wrong hash type: Invalid serialization data for Closure object
unknown class: Invalid serialization data for Closure object (cannot load class "NoSuchClass")
internal class: Invalid serialization data for Closure object (cannot load class "stdClass")
unknown site: Invalid serialization data for Closure object (constant-expression closure "$nope@0" of class Demo not found)
unknown rank: Invalid serialization data for Closure object (constant-expression closure "$p@9" of class Demo not found)
stale hash: Invalid serialization data for Closure object (constant-expression closure "$p@0" of class Demo changed)
Cannot unserialize an already initialized Closure
