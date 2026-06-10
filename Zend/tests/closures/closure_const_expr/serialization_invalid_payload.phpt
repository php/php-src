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
// "<site>@<rank>#<hash>": the hash suffix verifies the closure's code. A
// producer that cannot compute it may omit it; resolution is then positional.
$id = (new ReflectionFunction($closure))->getConstExprId();
// Flip the last hex nibble of the code hash (staying int-safe on 32-bit).
$stale = '$p@0#' . substr($id, -8, 7) . dechex(hexdec(substr($id, -1)) ^ 1);

// Serialize an array and rebrand it as a Closure object payload, so we can
// feed __unserialize() arbitrary tagged-union shapes.
$obj = static fn (array $data): string => 'O:7:"Closure":' . substr(serialize($data), 2);

// Tagged-union payload: [ [], ["const-expr", [class, id]] ].
$mk = static fn (string $class, string $id): string => $obj([[], ['const-expr', [$class, $id]]]);

// Sanity checks: a full reference works, and so does a hash-less one.
var_dump(unserialize($mk('Demo', $id))());
var_dump(unserialize($mk('Demo', '$p@0'))());

$payloads = [
    'empty data' => $obj([]),
    'one element' => $obj([[]]),
    'unknown tag' => $obj([[], ['whatever', ['Demo', $id]]]),
    'tag not list' => $obj([[], 'const-expr']),
    'wrong id type' => $obj([[], ['const-expr', ['Demo', 0]]]),
    'three-slot reference' => $obj([[], ['const-expr', ['Demo', $id, 1]]]),
    'unknown class' => $mk('NoSuchClass', $id),
    'internal class' => $mk('stdClass', $id),
    'unknown site' => $mk('Demo', '$nope@0'),
    'malformed id' => $mk('Demo', 'no-at-sign'),
    'malformed hash' => $mk('Demo', '$p@0#xyzwxyzw'),
    'short hash' => $mk('Demo', '$p@0#1234'),
    'zero hash' => $mk('Demo', '$p@0#00000000'),
    'stale hash' => $mk('Demo', $stale),
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
    $closure->__unserialize([[], ['const-expr', ['Demo', $id]]]);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
string(2) "ok"
string(2) "ok"
empty data: Invalid serialization data for Closure object
one element: Invalid serialization data for Closure object
unknown tag: Invalid serialization data for Closure object
tag not list: Invalid serialization data for Closure object
wrong id type: Invalid serialization data for Closure object
three-slot reference: Invalid serialization data for Closure object
unknown class: Invalid serialization data for Closure object (cannot load class "NoSuchClass")
internal class: Invalid serialization data for Closure object (cannot load class "stdClass")
unknown site: Invalid serialization data for Closure object (constant-expression closure "$nope@0" of class Demo not found)
malformed id: Invalid serialization data for Closure object (constant-expression closure "no-at-sign" of class Demo not found)
malformed hash: Invalid serialization data for Closure object (constant-expression closure "$p@0#xyzwxyzw" of class Demo not found)
short hash: Invalid serialization data for Closure object (constant-expression closure "$p@0#1234" of class Demo not found)
zero hash: Invalid serialization data for Closure object (constant-expression closure "$p@0#00000000" of class Demo not found)
stale hash: Invalid serialization data for Closure object (constant-expression closure "$p@0#%s" of class Demo changed)
Cannot unserialize an already initialized Closure
