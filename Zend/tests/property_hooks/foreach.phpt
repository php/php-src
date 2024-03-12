--TEST--
foreach over hooked properties
--FILE--
<?php

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
}

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
          return $field;
        }
        set {
          echo __METHOD__, "\n";
          $field = $value;
        }
    }
    public string $backedUninitialized {
        get {
          echo __METHOD__, "\n";
          $field ??= 'backedUninitialized';
          return $field;
        }
        set {
          echo __METHOD__, "\n";
          $field = $value;
        }
    }
}

function testByRef($object) {
    foreach ($object as $prop => &$value) {
        echo "$prop => $value\n";
        $value = strtoupper($value);
    }
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

?>
--EXPECT--
ByVal::$virtualByVal::get
virtualByVal => virtualByVal
ByVal::$virtualByVal::set
ByVal::$backed::get
backed => backed
ByVal::$backed::set
ByVal::$backedUninitialized::get
backedUninitialized => backedUninitialized
ByVal::$backedUninitialized::set
plain => plain
ByRef::$virtualByRef::get
virtualByRef => virtualByRef
ByRef::$virtualByRef::set
object(ByVal)#1 (5) {
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
}
plain => plain
ByRef::$virtualByRef::get
virtualByRef => virtualByRef
ByRef::$virtualByRef::set
object(ByRef)#1 (2) {
  ["plain"]=>
  string(5) "PLAIN"
  ["_virtualByRef":"ByRef":private]=>
  string(12) "VIRTUALBYREF"
}
plain => plain
ByRef::$virtualByRef::get
virtualByRef => virtualByRef
object(ByRef)#1 (2) {
  ["plain"]=>
  string(5) "PLAIN"
  ["_virtualByRef":"ByRef":private]=>
  &string(12) "VIRTUALBYREF"
}
