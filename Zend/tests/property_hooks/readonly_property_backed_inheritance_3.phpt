--TEST--
Backed readonly property get() in child class behaves as expected
--FILE--
<?php

class ParentClass {
    public function __construct(
        public readonly int $prop
    ) {}

    public function getParentValue() {
        echo "ParentClass::getParentValue(): {$this->prop}\n";
        var_dump($this);
        return $this->prop;
    }
}

class ChildClass extends ParentClass {

    public readonly int $prop {
        get {
            echo 'In ChildClass::$prop::get():' . "\n";
            echo '    parent::$prop::get(): ' . parent::$prop::get() . "\n";
            echo '    $this->prop: ' . $this->prop . "\n";
            echo '    $this->prop * 2: ' . $this->prop * 2 . "\n";
            return $this->prop * 2;
        }
        set => $value;
    }

    public function setAgain() {
        $this->prop = 42;
    }
}

$t = new ChildClass(911);

echo "\nFirst call:\n";
$t->prop;

echo "\nFirst call didn't change state:\n";
$t->prop;

echo "\nUnderlying value never touched:\n";
var_dump($t);

echo "\nCalling scope is child, hitting child get() and child state expected:\n";
$t->getParentValue();

try {
    $t->setAgain(); // cannot write, readonly
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    $t->prop = 43; // cannot write, visibility
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
First call:
In ChildClass::$prop::get():
    parent::$prop::get(): 911
    $this->prop: 911
    $this->prop * 2: 1822

First call didn't change state:
In ChildClass::$prop::get():
    parent::$prop::get(): 911
    $this->prop: 911
    $this->prop * 2: 1822

Underlying value never touched:
object(ChildClass)#1 (1) {
  ["prop"]=>
  int(911)
}

Calling scope is child, hitting child get() and child state expected:
In ChildClass::$prop::get():
    parent::$prop::get(): 911
    $this->prop: 911
    $this->prop * 2: 1822
ParentClass::getParentValue(): 1822
object(ChildClass)#1 (1) {
  ["prop"]=>
  int(911)
}
In ChildClass::$prop::get():
    parent::$prop::get(): 911
    $this->prop: 911
    $this->prop * 2: 1822
Cannot modify readonly property ChildClass::$prop
Cannot modify protected(set) readonly property ChildClass::$prop from global scope
