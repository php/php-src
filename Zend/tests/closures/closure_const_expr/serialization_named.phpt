--TEST--
First-class callable references serialize as name-keyed declaration sites
--FILE--
<?php

namespace App {
    function helper(): string { return 'ns-helper'; }
}

namespace {

#[Attribute(Attribute::TARGET_ALL)]
class A {
    public function __construct(public mixed $cb = null) {}
}

class Validators {
    public static function check(): string { return 'cross-class'; }
}

class Base {
    protected static function prot(): string { return 'prot-' . static::class; }
}

class Demo extends Base {
    #[A(self::priv(...))]
    #[A(self::prot(...))]
    #[A(parent::prot(...))]
    #[A(Validators::check(...))]
    #[A(strlen(...))]
    #[A(App\helper(...))]
    public int $p = 0;

    // First-class callable references consume no rank: the anonymous closure
    // that follows one is still @0.
    #[A(strlen(...))]
    #[A(static function () { return 'anon'; })]
    public int $q = 0;

    private static function priv(): string { return 'priv'; }
}

$attrs = (new ReflectionProperty(Demo::class, 'p'))->getAttributes();
foreach ($attrs as $i => $attr) {
    $closure = $attr->getArguments()[0];
    $payload = serialize($closure);
    $u = unserialize($payload);
    $args = $i === 4 ? ['abcd'] : [];
    // The id is "<site>@Called::method" / "<site>@function", the payload has
    // no line field, and the recreated closure behaves identically
    // (including static:: binding) and re-serializes byte-identically.
    var_dump(
        (new ReflectionFunction($closure))->getConstExprId()
        . ' | ' . var_export($u(...$args) === $closure(...$args), true)
        . ' | ' . var_export(serialize($u) === $payload, true)
    );
}
echo serialize($attrs[0]->getArguments()[0]), "\n";

// parent:: and self:: forms resolve their distinct static:: bindings.
var_dump(unserialize(serialize($attrs[1]->getArguments()[0]))());
var_dump(unserialize(serialize($attrs[2]->getArguments()[0]))());

// No rank is consumed by first-class callable references.
$qattrs = (new ReflectionProperty(Demo::class, 'q'))->getAttributes();
var_dump(preg_replace('/#[0-9a-f]{8}$/', '', (new ReflectionFunction($qattrs[1]->getArguments()[0]))->getConstExprId()));

// Runtime-created named closures are not declaration sites and refuse,
// even when an identical reference exists in an attribute.
foreach ([strlen(...), Validators::check(...), Closure::fromCallable('strlen')] as $closure) {
    try {
        serialize($closure);
        echo "serialized!?\n";
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
}

// The name is the address, the declaration is the guard: keys that no
// element declares do not resolve, whatever they name.
foreach (['$p@system', '$p@Demo::nope', '$q@Validators::check', '$p@0'] as $id) {
    try {
        Closure::fromConstExpr('Demo', $id);
        echo "resolved!?\n";
    } catch (ValueError $e) {
        echo $id, ": ", $e->getMessage(), "\n";
    }
}
try {
    unserialize('O:7:"Closure":2:' . substr(serialize([[], ['const-expr', ['Demo', '$p@system']]]), 4));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

}
?>
--EXPECT--
string(27) "$p@Demo::priv | true | true"
string(27) "$p@Demo::prot | true | true"
string(27) "$p@Base::prot | true | true"
string(34) "$p@Validators::check | true | true"
string(23) "$p@strlen | true | true"
string(27) "$p@App\helper | true | true"
O:7:"Closure":2:{i:0;a:0:{}i:1;a:2:{i:0;s:10:"const-expr";i:1;a:2:{i:0;s:4:"Demo";i:1;s:13:"$p@Demo::priv";}}}
string(9) "prot-Demo"
string(9) "prot-Base"
string(4) "$q@0"
Serialization of 'Closure' is not allowed
Serialization of 'Closure' is not allowed
Serialization of 'Closure' is not allowed
$p@system: Closure::fromConstExpr(): Argument #2 ($id) does not refer to a constant-expression closure of class Demo
$p@Demo::nope: Closure::fromConstExpr(): Argument #2 ($id) does not refer to a constant-expression closure of class Demo
$q@Validators::check: Closure::fromConstExpr(): Argument #2 ($id) does not refer to a constant-expression closure of class Demo
$p@0: Closure::fromConstExpr(): Argument #2 ($id) does not refer to a constant-expression closure of class Demo
Invalid serialization data for Closure object (constant-expression closure "$p@system" of class Demo not found)
