--TEST--
ZE2 __set() and __get()
--FILE--
<?php
class setter {
	public $n;
	public $x = array('a' => 1, 'b' => 2, 'c' => 3);

	function __get($nm) {
		echo "Getting [$nm]\n";

		if (isset($this->x[$nm])) {
			$r = $this->x[$nm];
			echo "Returning: $r\n";
			return $r;
		} 
		else {
			echo "Nothing!\n";
		}
	}

	function __set($nm, $val) {
		echo "Setting [$nm] to $val\n";
                    
		if (isset($this->x[$nm])) {
			$this->x[$nm] = $val;
			echo "OK!\n";
		} 
		else {
			echo "Not OK!\n";
		}
	}
}

$foo = new Setter();

// this doesn't go through __set()... should it?
$foo->n = 1;

// the rest are fine...
$foo->a = 100;
$foo->a++;
$foo->z++;
var_dump($foo);
        
?>
--EXPECTF--
Setting [a] to 100
OK!
Getting [a]
Returning: 100
Setting [a] to 101
OK!
Getting [z]
Nothing!
Setting [z] to 1
Not OK!
object(setter)#%d (2) {
  ["n"]=>
  int(1)
  ["x"]=>
  array(3) {
    ["a"]=>
    int(101)
    ["b"]=>
    int(2)
    ["c"]=>
    int(3)
  }
}
