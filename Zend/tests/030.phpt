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
object(Exception)#%d (7) {
  ["message":protected]=>
  string(3) "foo"
  ["string":"Exception":private]=>
  string(0) ""
  ["code":protected]=>
  int(0)
  ["file":protected]=>
  string(%d) "%s030.php"
  ["line":protected]=>
  int(%d)
  ["trace":"Exception":private]=>
  array(1) {
    [0]=>
    array(6) {
      ["file"]=>
      string(%d) "%s030.php"
      ["line"]=>
      int(%d)
      ["function"]=>
      string(3) "bar"
      ["class"]=>
      string(3) "foo"
      ["type"]=>
      string(2) "->"
      ["args"]=>
      array(0) {
      }
    }
  }
  ["previous":"Exception":private]=>
  NULL
}
'test' => '0'
'test_2' => '1'
'test_3' => '2'
ok
