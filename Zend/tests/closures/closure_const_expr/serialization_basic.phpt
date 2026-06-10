--TEST--
Closures in constant expressions are serializable as declaration-site references
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL)]
class A {
    public function __construct(public mixed $cb = null, public mixed $extra = null) {}
}

#[A(static function () { return 'class'; })]
class Demo {
    #[A(static function () { return 'const'; })]
    public const FOO = 1;

    #[A(cb: [static function () { return 'prop-1'; }, static function (): string { return 'prop-2'; }])]
    public string $name = '';

    #[A(static function () { return 'method'; })]
    public function m(
        #[A(static function () { return 'param'; })]
        int $x = 0,
    ): void {}
}

enum E {
    #[A(static function () { return 'case'; })]
    case X;
}

$closures = [
    'class' => (new ReflectionClass(Demo::class))->getAttributes()[0]->getArguments()[0],
    'const' => (new ReflectionClassConstant(Demo::class, 'FOO'))->getAttributes()[0]->getArguments()[0],
    'prop-1' => (new ReflectionProperty(Demo::class, 'name'))->getAttributes()[0]->getArguments()['cb'][0],
    'prop-2' => (new ReflectionProperty(Demo::class, 'name'))->getAttributes()[0]->getArguments()['cb'][1],
    'method' => (new ReflectionMethod(Demo::class, 'm'))->getAttributes()[0]->getArguments()[0],
    'param' => (new ReflectionParameter([Demo::class, 'm'], 'x'))->getAttributes()[0]->getArguments()[0],
    'case' => (new ReflectionClassConstant(E::class, 'X'))->getAttributes()[0]->getArguments()[0],
];

// The reference lives in the __serialize() payload: [ [], ["const-expr",
// [class, site, key, hash]] ]. These read it out and rebuild a closure from a
// bare reference the way an exporter would.
$refOf = static fn (Closure $c) => $c->__serialize()[1][1];   // [class, site, key, hash]
$fromRef = static fn (array $ref) => unserialize('O:7:"Closure":' . substr(serialize([[], ['const-expr', $ref]]), 2));

foreach ($closures as $expected => $closure) {
    $unserialized = unserialize(serialize($closure));
    $recreated = $fromRef($refOf($closure));

    var_dump($expected === $closure() && $expected === $unserialized() && $expected === $recreated());
}

// References are assigned in canonical walk order: class attributes first, then
// constants, then properties, then methods (including parameters). The display
// below is "<site>@<key>"; the code hash (the fourth field) is not part of the
// addressing contract (see serialization_code_hash.phpt).
$ids = array_map(
    static fn ($c) => $refOf($c)[1] . '@' . $refOf($c)[2],
    $closures
);
var_dump($ids);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
array(7) {
  ["class"]=>
  string(2) "@0"
  ["const"]=>
  string(5) "FOO@0"
  ["prop-1"]=>
  string(7) "$name@0"
  ["prop-2"]=>
  string(7) "$name@1"
  ["method"]=>
  string(5) "m()@0"
  ["param"]=>
  string(5) "m()@1"
  ["case"]=>
  string(3) "X@0"
}
