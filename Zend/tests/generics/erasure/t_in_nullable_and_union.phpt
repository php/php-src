--TEST--
Erasure: T inside nullable and union types collapses redundant nulls/mixed
--FILE--
<?php
function nullableUnbounded<T>(?T $x): ?T { return $x; }
function nullableBounded<T : int>(?T $x): ?T { return $x; }
function unionNullUnbounded<T>(null|T $x): null|T { return $x; }
function unionMultiUnbounded<T>(int|bool|T $x): int|bool|T { return $x; }
function unionNullBounded<T : int>(null|T $x): null|T { return $x; }
function unionMultiBounded<T : Stringable>(int|T $x): int|T { return $x; }

class Holder<T> {
    public function __construct(public ?T $value) {}
    public function set(?T $v): void { $this->value = $v; }
    public function get(): ?T { return $this->value; }
}

var_dump(nullableUnbounded(42));
var_dump(nullableUnbounded(null));
var_dump(nullableUnbounded("hi"));
var_dump(nullableBounded(7));
var_dump(nullableBounded(null));
var_dump(unionNullUnbounded(null));
var_dump(unionNullUnbounded("x"));
var_dump(unionMultiUnbounded(1));
var_dump(unionMultiUnbounded(true));
var_dump(unionMultiUnbounded(new stdClass));
var_dump(unionNullBounded(null));
var_dump(unionNullBounded(7));
var_dump(unionMultiBounded(1));

echo "nullableUnbounded: ",  (new ReflectionFunction('nullableUnbounded'))->getReturnType(),  "\n";
echo "nullableBounded: ",    (new ReflectionFunction('nullableBounded'))->getReturnType(),    "\n";
echo "unionNullUnbounded: ", (new ReflectionFunction('unionNullUnbounded'))->getReturnType(), "\n";
echo "unionMultiUnbounded: ", (new ReflectionFunction('unionMultiUnbounded'))->getReturnType(), "\n";
echo "unionNullBounded: ",   (new ReflectionFunction('unionNullBounded'))->getReturnType(),   "\n";
echo "unionMultiBounded: ",  (new ReflectionFunction('unionMultiBounded'))->getReturnType(),  "\n";

$rc = new ReflectionClass(Holder::class);
echo "Holder property: ", $rc->getProperty('value')->getType(), "\n";
echo "Holder set param: ", $rc->getMethod('set')->getParameters()[0]->getType(), "\n";
echo "Holder get return: ", $rc->getMethod('get')->getReturnType(), "\n";

$h = new Holder(1);
$h->set(null);
var_dump($h->get());
?>
--EXPECT--
int(42)
NULL
string(2) "hi"
int(7)
NULL
NULL
string(1) "x"
int(1)
bool(true)
object(stdClass)#1 (0) {
}
NULL
int(7)
int(1)
nullableUnbounded: mixed
nullableBounded: ?int
unionNullUnbounded: mixed
unionMultiUnbounded: mixed
unionNullBounded: ?int
unionMultiBounded: Stringable|int
Holder property: mixed
Holder set param: mixed
Holder get return: mixed
NULL
