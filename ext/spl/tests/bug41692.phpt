--TEST--
Bug #41692 (ArrayObject shows weird behaviour in respect to inheritance)
--FILE--
<?php

class Bar extends ArrayObject {
    private $foo = array( 1, 2, 3 );
    function __construct()
    {
        parent::__construct($this->foo);
    }
}

$foo = new Bar();
var_dump($foo);
$foo['foo'] = 23;

$bar = new Bar();
var_dump($bar);

echo "Done\n";
?>
--EXPECTF--	
object(Bar)#%d (2) {
  ["foo":"Bar":private]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
object(Bar)#%d (2) {
  ["foo":"Bar":private]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
  ["storage":"ArrayObject":private]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
Done
