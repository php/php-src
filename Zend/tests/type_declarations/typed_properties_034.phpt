--TEST--
Test typed properties passed to typed function
--FILE--
<?php
$foo = new class {
    public ?int $bar = 42;
    public int $baz;

    public function &getIterator() {
        foreach (['1', &$this->bar] as &$item) {
            yield $item;
        }
    }
};

function foo(?int &$a) {
    var_dump($a);
    $a = null;
}

foo($foo->bar);

try {
    $foo->baz = &$foo->bar;
} catch (Error $e) { echo $e->getMessage(), "\n"; }
$foo->bar = 10;

foreach ($foo->getIterator() as &$item) {
    $foo->baz = &$item;
    var_dump($foo->baz);
}

try {
    foo($foo->bar);
} catch (Error $e) { echo $e->getMessage(), "\n"; }

var_dump($foo);
?>
--EXPECT--
int(42)
Cannot assign null to property class@anonymous::$baz of type int
int(1)
int(10)
int(10)
Cannot assign null to reference held by property class@anonymous::$baz of type int
object(class@anonymous)#1 (2) {
  ["bar"]=>
  &int(10)
  ["baz"]=>
  &int(10)
}
