--TEST--
foreach over hooked properties
--FILE--
<?php

#[AllowDynamicProperties]
class ByRef {
    public $plain = 'plain';
    private $_virtualByRef = 'virtualByRef';
    public $virtualByRef {
        &get {
          echo __METHOD__, "\n";
          return $this->_virtualByRef;
        }
        set {
          echo __METHOD__, "\n";
          $this->_virtualByRef = $value;
        }
    }
    public $virtualSetOnly {
        set {
            echo __METHOD__, "\n";
        }
    }
    public function __construct() {
        $this->undef = 'dynamic';
        $this->dynamic = 'dynamic';
        unset($this->undef);
    }
}

#[AllowDynamicProperties]
class ByVal extends ByRef {
    private $_virtualByVal = 'virtualByVal';
    public $virtualByVal {
        get {
          echo __METHOD__, "\n";
          return $this->_virtualByVal;
        }
        set {
          echo __METHOD__, "\n";
          $this->_virtualByVal = $value;
        }
    }
    public $backed = 'backed' {
        get {
          echo __METHOD__, "\n";
          return $this->backed;
        }
        set {
          echo __METHOD__, "\n";
          $this->backed = $value;
        }
    }
    public string $backedUninitialized {
        get {
          echo __METHOD__, "\n";
          $this->backedUninitialized ??= 'backedUninitialized';
          return $this->backedUninitialized;
        }
        set {
          echo __METHOD__, "\n";
          $this->backedUninitialized = $value;
        }
    }
}

function testByRef($object) {
    foreach ($object as $prop => &$value) {
        echo "$prop => $value\n";
        $value = strtoupper($value);
    }
    unset($value);
    var_dump($object);
}

function testByVal($object) {
    foreach ($object as $prop => $value) {
        echo "$prop => $value\n";
        $object->{$prop} = strtoupper($value);
    }
    var_dump($object);
}

testByVal(new ByVal);
testByVal(new ByRef);
testByRef(new ByRef);

class A {
    private $changed { get => 'A'; }
    protected $promoted { get => 'A'; }
    protected $protected { get => 'A'; }
    private $shadowed = 'A';

    public function test() {
        foreach ($this as $k => $v) {
            var_dump($k, $v);
        }
    }
}

#[AllowDynamicProperties]
class B extends A {
    public $changed { get => 'B'; }
    public $promoted { get => 'B'; }
}

$b = new B;
$b->shadowed = 'Global';
$b->test();

?>
--EXPECTF--
plain => plain
ByRef::$virtualByRef::get
virtualByRef => virtualByRef
ByRef::$virtualByRef::set
ByVal::$virtualByVal::get
virtualByVal => virtualByVal
ByVal::$virtualByVal::set
ByVal::$backed::get
backed => backed
ByVal::$backed::set
ByVal::$backedUninitialized::get
backedUninitialized => backedUninitialized
ByVal::$backedUninitialized::set
dynamic => dynamic
object(ByVal)#%d (6) {
  ["plain"]=>
  string(5) "PLAIN"
  ["_virtualByRef":"ByRef":private]=>
  string(12) "VIRTUALBYREF"
  ["_virtualByVal":"ByVal":private]=>
  string(12) "VIRTUALBYVAL"
  ["backed"]=>
  string(6) "BACKED"
  ["backedUninitialized"]=>
  string(19) "BACKEDUNINITIALIZED"
  ["dynamic"]=>
  string(7) "DYNAMIC"
}
plain => plain
ByRef::$virtualByRef::get
virtualByRef => virtualByRef
ByRef::$virtualByRef::set
dynamic => dynamic
object(ByRef)#%d (3) {
  ["plain"]=>
  string(5) "PLAIN"
  ["_virtualByRef":"ByRef":private]=>
  string(12) "VIRTUALBYREF"
  ["dynamic"]=>
  string(7) "DYNAMIC"
}
plain => plain
ByRef::$virtualByRef::get
virtualByRef => virtualByRef
dynamic => dynamic
object(ByRef)#%d (3) {
  ["plain"]=>
  string(5) "PLAIN"
  ["_virtualByRef":"ByRef":private]=>
  string(12) "VIRTUALBYREF"
  ["dynamic"]=>
  string(7) "DYNAMIC"
}
string(7) "changed"
string(1) "A"
string(8) "promoted"
string(1) "B"
string(9) "protected"
string(1) "A"
string(8) "shadowed"
string(1) "A"
