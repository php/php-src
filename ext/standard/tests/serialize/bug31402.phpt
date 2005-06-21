--TEST--
Bug #31402 (unserialize() generates references when it should not)
--FILE--
<?php 

class X {
  public $i;

  function X($i) {
    $this->i = $i;
  }
}

class Y {
  public $A = array();
  public $B;

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
--EXPECTF--
object(Y)#%d (2) {
  ["A"]=>
  array(2) {
    [1]=>
    object(X)#%d (1) {
      ["i"]=>
      int(1)
    }
    [2]=>
    object(X)#%d (1) {
      ["i"]=>
      int(2)
    }
  }
  ["B"]=>
  object(X)#%d (1) {
    ["i"]=>
    int(1)
  }
}
object(Y)#%d (2) {
  ["A"]=>
  array(2) {
    [1]=>
    object(X)#%d (1) {
      ["i"]=>
      int(1)
    }
    [2]=>
    object(X)#%d (1) {
      ["i"]=>
      int(2)
    }
  }
  ["B"]=>
  object(X)#%d (1) {
    ["i"]=>
    int(1)
  }
}
