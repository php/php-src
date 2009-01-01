--TEST--
Closure 020: Trying to access private property outside class
--FILE--
<?php

class foo {
	private $test = 3;
	
	public function x() {
		$a = &$this;
		$this->a = function() use (&$a) { return $a; };
		var_dump($this->a->__invoke());
		var_dump(is_a($this->a, 'closure'));
		var_dump(is_callable($this->a));
		
		return $this->a;
	}
}

$foo = new foo;
$y = $foo->x();
var_dump($y()->test);

?>
--EXPECTF--
object(foo)#%d (%d) {
  [u"test":u"foo":private]=>
  int(3)
  [u"a"]=>
  object(Closure)#%d (2) {
    ["this"]=>
    object(foo)#%d (2) {
      [u"test":u"foo":private]=>
      int(3)
      [u"a"]=>
      object(Closure)#%d (2) {
        ["this"]=>
        *RECURSION*
        ["static"]=>
        array(1) {
          [u"a"]=>
          *RECURSION*
        }
      }
    }
    ["static"]=>
    array(1) {
      [u"a"]=>
      &object(foo)#%d (2) {
        [u"test":u"foo":private]=>
        int(3)
        [u"a"]=>
        object(Closure)#%d (2) {
          ["this"]=>
          *RECURSION*
          ["static"]=>
          array(1) {
            [u"a"]=>
            *RECURSION*
          }
        }
      }
    }
  }
}
bool(true)
bool(true)

Fatal error: Cannot access private property foo::$test in %s on line %d
