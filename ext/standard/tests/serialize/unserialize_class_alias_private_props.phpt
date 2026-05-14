--TEST--
unserialize() respects class_alias for private properties
--FILE--
<?php

// Test 1: Basic class_alias with private property
class HelloAlias {
    public function __construct(
        private readonly int $answer = 0
    ) {}

    public function getAnswer(): int {
        return $this->answer;
    }
}
class_alias(HelloAlias::class, 'Hello');

$serialized = 'O:5:"Hello":1:{s:13:"' . "\0Hello\0" . 'answer";i:42;}';
$obj = unserialize($serialized);
var_dump($obj instanceof HelloAlias);
var_dump($obj->getAnswer());

// Test 2: Protected property (should continue to work)
class ProtoAlias {
    public function __construct(
        protected int $value = 0
    ) {}
    public function getValue(): int { return $this->value; }
}
class_alias(ProtoAlias::class, 'Proto');

$serialized2 = 'O:5:"Proto":1:{s:8:"' . "\0*\0" . 'value";i:99;}';
$obj2 = unserialize($serialized2);
var_dump($obj2 instanceof ProtoAlias);
var_dump($obj2->getValue());

// Test 3: Public property (should continue to work)
class PubAlias {
    public int $data = 0;
}
class_alias(PubAlias::class, 'Pub');

$serialized3 = 'O:3:"Pub":1:{s:4:"data";i:77;}';
$obj3 = unserialize($serialized3);
var_dump($obj3 instanceof PubAlias);
var_dump($obj3->data);

echo "Done\n";
?>
--EXPECT--
bool(true)
int(42)
bool(true)
int(99)
bool(true)
int(77)
Done
