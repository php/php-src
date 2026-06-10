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
    // The reference is [class, site, key, hash]; for a first-class callable the
    // key is "Called::method" / "function" and the hash is 0. The recreated
    // closure behaves identically (including static:: binding) and re-serializes
    // byte-identically.
    $ref = $closure->__serialize()[1][1];
    var_dump(
        $ref[1] . '@' . $ref[2]
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
$qref = $qattrs[1]->getArguments()[0]->__serialize()[1][1];
var_dump($qref[1] . '@' . $qref[2]);

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

// The name is the address, the declaration is the guard: references that no
// element declares do not resolve, whatever they name.
foreach ([['$p', 'system', 0], ['$p', 'Demo::nope', 0], ['$q', 'Validators::check', 0], ['$p', 0, 0]] as [$site, $key, $hash]) {
    try {
        unserialize('O:7:"Closure":' . substr(serialize([[], ['const-expr', ['Demo', $site, $key, $hash]]]), 2));
        echo "resolved!?\n";
    } catch (Exception $e) {
        echo "$site@$key: ", $e->getMessage(), "\n";
    }
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
O:7:"Closure":2:{i:0;a:0:{}i:1;a:2:{i:0;s:10:"const-expr";i:1;a:4:{i:0;s:4:"Demo";i:1;s:2:"$p";i:2;s:10:"Demo::priv";i:3;i:0;}}}
string(9) "prot-Demo"
string(9) "prot-Base"
string(4) "$q@0"
Serialization of 'Closure' is not allowed
Serialization of 'Closure' is not allowed
Serialization of 'Closure' is not allowed
$p@system: Invalid serialization data for Closure object (constant-expression closure "$p@system" of class Demo not found)
$p@Demo::nope: Invalid serialization data for Closure object (constant-expression closure "$p@Demo::nope" of class Demo not found)
$q@Validators::check: Invalid serialization data for Closure object (constant-expression closure "$q@Validators::check" of class Demo not found)
$p@0: Invalid serialization data for Closure object (constant-expression closure "$p@0" of class Demo not found)
