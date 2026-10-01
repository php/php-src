--TEST--
SplFixedArray: subclass not calling parent::__construct() (GH-23811)
--DESCRIPTION--
The internal struct is zeroed on object creation, which used to leave the
"resize in progress" sentinel at 0 instead of -1. setSize() then took the
re-entrancy early return and silently did nothing, leaving the array stuck
at size 0 for the lifetime of the object.
--FILE--
<?php
class Unconstructed extends SplFixedArray {
    public function __construct() {
        /* deliberately does not call parent::__construct() */
    }
}

$a = new Unconstructed();
echo "initial: ", $a->getSize(), "\n";

$a->setSize(3);
echo "after setSize(3): ", $a->getSize(), "\n";

$a[0] = "x";
$a[2] = "z";
var_dump($a->toArray());

$a->setSize(1);
echo "after setSize(1): ", $a->getSize(), "\n";

$a->setSize(0);
echo "after setSize(0): ", $a->getSize(), "\n";

/* Deferred initialisation: calling the parent constructor later still works. */
class LateInit extends SplFixedArray {
    public function __construct() {
    }
    public function init(int $size): void {
        parent::__construct($size);
    }
}
$b = new LateInit();
$b->init(2);
echo "deferred parent::__construct(2): ", $b->getSize(), "\n";

/* Cloning one of these must also yield a resizable array. */
$c = clone new Unconstructed();
$c->setSize(2);
echo "clone then setSize(2): ", $c->getSize(), "\n";
?>
--EXPECT--
initial: 0
after setSize(3): 3
array(3) {
  [0]=>
  string(1) "x"
  [1]=>
  NULL
  [2]=>
  string(1) "z"
}
after setSize(1): 1
after setSize(0): 0
deferred parent::__construct(2): 2
clone then setSize(2): 2
