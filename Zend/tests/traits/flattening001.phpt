--TEST--
Methods using object properties
--FILE--
<?php
error_reporting(E_ALL);

trait T1 {
  public function getText() {
    return $this->text;
  }
}

trait T2 {
  public function setTextT2($val) {
    $this->text = $val;
  }
}

class TraitsTest {
  use T1;
  use T2;
  private $text = 'test';
  public function setText($val) {
    $this->text = $val;
  }
}

$o = new TraitsTest();
var_dump($o->getText());

$o->setText('foo');

var_dump($o->getText());

$o->setText('bar');

var_dump($o->getText());
?>
--EXPECTF--	
string(4) "test"
string(3) "foo"
string(3) "bar"