--TEST--
Bug #31402 (unserialize() generates references when it should not)
--FILE--
<?php 

class X {
  var $i;

  function X($i) {
    $this->i = $i;
  }
}

class Y {
  var $A = array();
  var $B;

  function Y() {
    $this->A[1] = new X(1);
    $this->A[2] = new X(2);
    $this->B = $this->A[1];
  }
}

$before = new Y();
$ser = serialize($before);
$after = unserialize($ser);

var_dump($before, $after);

?>
--EXPECT--
object(y)(2) {
  ["A"]=>
  array(2) {
    [1]=>
    object(x)(1) {
      ["i"]=>
      int(1)
    }
    [2]=>
    object(x)(1) {
      ["i"]=>
      int(2)
    }
  }
  ["B"]=>
  object(x)(1) {
    ["i"]=>
    int(1)
  }
}
object(y)(2) {
  ["A"]=>
  array(2) {
    [1]=>
    object(x)(1) {
      ["i"]=>
      int(1)
    }
    [2]=>
    object(x)(1) {
      ["i"]=>
      int(2)
    }
  }
  ["B"]=>
  object(x)(1) {
    ["i"]=>
    int(1)
  }
}
