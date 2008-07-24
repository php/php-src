--TEST--
Bug #45178 memory corruption on assignment result of "new" by reference
--FILE--
<?php
class Foo {
    function __construct() {
    	$this->error = array($this,$this);
    }
}
$a =& new Foo();

class Bar {
	function __construct() {
		$this->_rme2 = $this;
	}
}

$b =& new Bar();
$b->_rme2 = 0;
var_dump($b);
?>
--EXPECTF--
Strict Standards: Assigning the return value of new by reference is deprecated in %sbug45178.php on line 7

Strict Standards: Assigning the return value of new by reference is deprecated in %sbug45178.php on line 15
object(Bar)#%d (1) {
  ["_rme2"]=>
  int(0)
}
