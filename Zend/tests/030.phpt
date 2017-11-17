--TEST--
Overriding $this in catch and checking the object properties later.
--FILE--
<?php

class foo {
	public $test = 0;
	private $test_2 = 1;
	protected $test_3 = 2;
	
	public function bar() {
		try {
			throw new Exception('foo');
		} catch (Exception $this) {
			var_dump($this);
		}

		$this->baz();		
	}
	
	public function baz() {
		foreach ($this as $k => $v) {
			printf("'%s' => '%s'\n", $k, $v);
		}		
		print "ok\n";
	}
}

$test = new foo;
$test->bar();

?>
--EXPECTF--
Fatal error: Cannot re-assign $this in %s030.php on line 11
