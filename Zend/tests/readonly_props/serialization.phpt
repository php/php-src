--TEST--
Serialization of readonly properties
--FILE--
<?php

class Test {
    public function __construct(
        public readonly int $prop = 1,
    ) {}
}

var_dump($s = serialize(new Test));
var_dump(unserialize($s));

// Readonly properties receive no special handling.
// What happens during unserialization stays in unserialization.
var_dump(unserialize("O:4:\"Test\":1:{s:4:\"prop\";i:2;}"));
var_dump(unserialize("O:4:\"Test\":2:{s:4:\"prop\";i:2;s:4:\"prop\";i:3;}"));

class TestDefault {
    public readonly int $prop = 1;
}

var_dump($s = serialize(new TestDefault));
var_dump(unserialize($s));

var_dump(unserialize("O:11:\"TestDefault\":0:{}"));
var_dump(unserialize("O:11:\"TestDefault\":1:{s:4:\"prop\";i:2;}"));
var_dump(unserialize("O:11:\"TestDefault\":2:{s:4:\"prop\";i:2;s:4:\"prop\";i:3;}"));

class TestDefaultWithUnserialize {
    public readonly int $prop = 1;
    public public(set) readonly int $lock= 1;

    public function __unserialize(array $data): void {
        foreach ($data as $key => $value) {
            $this->{$key} = $value;
        }
    }
}

$testDefaultWithUnserialize = unserialize("O:26:\"TestDefaultWithUnserialize\":1:{s:4:\"prop\";i:2;}");
var_dump($testDefaultWithUnserialize);

try {
    $testDefaultWithUnserialize->prop = 3;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $testDefaultWithUnserialize->lock = 3;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
string(30) "O:4:"Test":1:{s:4:"prop";i:1;}"
object(Test)#1 (1) {
  ["prop"]=>
  int(1)
}
object(Test)#1 (1) {
  ["prop"]=>
  int(2)
}
object(Test)#1 (1) {
  ["prop"]=>
  int(3)
}
string(38) "O:11:"TestDefault":1:{s:4:"prop";i:1;}"
object(TestDefault)#1 (1) {
  ["prop"]=>
  int(1)
}
object(TestDefault)#1 (1) {
  ["prop"]=>
  int(1)
}
object(TestDefault)#1 (1) {
  ["prop"]=>
  int(2)
}
object(TestDefault)#1 (1) {
  ["prop"]=>
  int(3)
}
object(TestDefaultWithUnserialize)#1 (2) {
  ["prop"]=>
  int(2)
  ["lock"]=>
  int(1)
}
Error: Cannot modify readonly property TestDefaultWithUnserialize::$prop
Error: Cannot modify readonly property TestDefaultWithUnserialize::$lock
