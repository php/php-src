--TEST--
Typed references must be kept track of and always be only the intersection of the type currently holding that reference
--FILE--
<?php

$a = new class {
    public ?iterable $it = [];
    public ?array $a;
    public ?Traversable $t;
};

$ref = &$a->it;
$a->a = &$ref;

var_dump($ref);

try {
    $a->t = &$ref;
} catch (TypeError $e) { var_dump($e->getMessage()); }
var_dump($ref);

$a->it = [1]; // type is still assignable
var_dump($ref);

try {
    $ref = new ArrayIterator();
} catch (TypeError $e) { var_dump($e->getMessage()); }
var_dump($ref instanceof ArrayIterator);

unset($a->a);

$ref = null;

$a->t = &$ref;

try {
    $ref = [];
} catch (TypeError $e) { var_dump($e->getMessage()); }
var_dump($ref instanceof ArrayIterator);

$ref = new ArrayIterator();
var_dump($ref instanceof ArrayIterator);

?>
--EXPECT--
array(0) {
}
string(72) "Cannot assign array to property class@anonymous::$t of type ?Traversable"
array(0) {
}
array(1) {
  [0]=>
  int(1)
}
string(92) "Cannot assign ArrayIterator to reference held by property class@anonymous::$a of type ?array"
bool(false)
string(90) "Cannot assign array to reference held by property class@anonymous::$t of type ?Traversable"
bool(false)
bool(true)
