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

// Test 4: Inheritance — child class with parent's private property via alias
class ParentClass {
    public function __construct(
        private int $secret = 0
    ) {}
    public function getSecret(): int { return $this->secret; }
}

class ChildClass extends ParentClass {
    public function __construct(
        private int $childProp = 0,
        int $secret = 0
    ) {
        parent::__construct($secret);
    }
    public function getChildProp(): int { return $this->childProp; }
}
class_alias(ChildClass::class, 'Kid');

// Serialized with alias name for the child's private prop, canonical name for parent's
$serialized4 = 'O:3:"Kid":2:{s:14:"' . "\0Kid\0" . 'childProp";i:10;s:19:"' . "\0ParentClass\0" . 'secret";i:20;}';
$obj4 = unserialize($serialized4);
var_dump($obj4 instanceof ChildClass);
var_dump($obj4->getChildProp());
var_dump($obj4->getSecret());

// Test 5: Multiple private properties with alias
class MultiPropAlias {
    public function __construct(
        private int $x = 0,
        private string $y = '',
        private bool $z = false
    ) {}
    public function getX(): int { return $this->x; }
    public function getY(): string { return $this->y; }
    public function getZ(): bool { return $this->z; }
}
class_alias(MultiPropAlias::class, 'Multi');

$serialized5 = 'O:5:"Multi":3:{s:8:"' . "\0Multi\0" . 'x";i:1;s:8:"' . "\0Multi\0" . 'y";s:3:"abc";s:8:"' . "\0Multi\0" . 'z";b:1;}';
$obj5 = unserialize($serialized5);
var_dump($obj5 instanceof MultiPropAlias);
var_dump($obj5->getX());
var_dump($obj5->getY());
var_dump($obj5->getZ());

// Test 6: Canonical name still works (non-alias path, regression check)
$serialized6 = 'O:14:"MultiPropAlias":3:{s:17:"' . "\0MultiPropAlias\0" . 'x";i:5;s:17:"' . "\0MultiPropAlias\0" . 'y";s:2:"hi";s:17:"' . "\0MultiPropAlias\0" . 'z";b:0;}';
$obj6 = unserialize($serialized6);
var_dump($obj6 instanceof MultiPropAlias);
var_dump($obj6->getX());
var_dump($obj6->getY());
var_dump($obj6->getZ());

// Test 7: Shadowed private properties — alias must not cross-contaminate
// (Addresses concern from GH-18542: \0Alias\0prop and \0Parent\0prop refer
//  to different private properties at different inheritance levels)
class Base {
    public function __construct(
        private string $prop = ''
    ) {}
    public function getBaseProp(): string { return $this->prop; }
}

class Child extends Base {
    public function __construct(
        private string $prop = '',
        string $baseProp = ''
    ) {
        parent::__construct($baseProp);
    }
    public function getChildProp(): string { return $this->prop; }
}
class_alias(Child::class, 'ChildAlias');

// Serialize with alias name for Child's prop, canonical for Base's prop.
// These are TWO SEPARATE private properties that must not be confused.
$serialized7 = 'O:10:"ChildAlias":2:{s:16:"' . "\0ChildAlias\0" . 'prop";s:5:"child";s:10:"' . "\0Base\0" . 'prop";s:4:"base";}';
$obj7 = unserialize($serialized7);
var_dump($obj7 instanceof Child);
var_dump($obj7->getChildProp());
var_dump($obj7->getBaseProp());

// Test 8: IMSoP's scenario from GH-18542 — alias and canonical name for
// the SAME private property in a single class (no inheritance).
// Both \0Bravo\0value and \0BravoAlias\0value refer to the same declared property.
// The fix resolves both correctly; last one in serialized data wins.
class Bravo {
    public function __construct(
        private string $value = ''
    ) {}
    public function getValue(): string { return $this->value; }
}
class_alias(Bravo::class, 'BravoAlias');

// Only alias-mangled name
$serialized8 = 'O:10:"BravoAlias":1:{s:17:"' . "\0BravoAlias\0" . 'value";s:5:"alias";}';
$obj8 = unserialize($serialized8);
var_dump($obj8 instanceof Bravo);
var_dump($obj8->getValue());

// Only canonical name
$serialized8b = 'O:10:"BravoAlias":1:{s:12:"' . "\0Bravo\0" . 'value";s:9:"canonical";}';
$obj8b = unserialize($serialized8b);
var_dump($obj8b instanceof Bravo);
var_dump($obj8b->getValue());

// Both alias AND canonical in same payload — last write wins, no dynamic property
$serialized8c = 'O:10:"BravoAlias":2:{s:12:"' . "\0Bravo\0" . 'value";s:5:"first";s:17:"' . "\0BravoAlias\0" . 'value";s:4:"last";}';
$obj8c = unserialize($serialized8c);
var_dump($obj8c instanceof Bravo);
var_dump($obj8c->getValue());

echo "Done\n";
?>
--EXPECT--
bool(true)
int(42)
bool(true)
int(99)
bool(true)
int(77)
bool(true)
int(10)
int(20)
bool(true)
int(1)
string(3) "abc"
bool(true)
bool(true)
int(5)
string(2) "hi"
bool(false)
bool(true)
string(5) "child"
string(4) "base"
bool(true)
string(5) "alias"
bool(true)
string(9) "canonical"
bool(true)
string(4) "last"
Done
