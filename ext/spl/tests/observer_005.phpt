--TEST--
SPL: SplObjectStorage serialization & visibility
--FILE--
<?php

class TestClass
{
	public    $def = 24;
	public    $pub = 25;
	protected $pro = 26;
	private   $pri = 27;
	
	public function __construct($pub = 42, $pro = 43, $pri = 44)
	{
		$this->pub = $pub;
		$this->pro = $pro;
		$this->pri = $pri;
	}
}

class ExtTestClass
{
}

class MyStorage extends SplObjectStorage
{
	public    $def = 24;
	public    $pub = 25;
	protected $pro = 26;
	private   $pri = 27;
	
	public function __construct($pub = 52, $pro = 53, $pri = 54)
	{
		$this->pub = $pub;
		$this->pro = $pro;
		$this->pri = $pri;
	}
}

class ExtStorage extends MyStorage
{
}

$storage = new MyStorage(1,2,3);

foreach(array(array(4,5,6),array(7,8,9)) as $value)
{
     $storage->attach(new TestClass($value[0], $value[1], $value[2]));
}

var_dump(count($storage));

foreach($storage as $object)
{
	var_dump($object);
}

var_dump($storage);

var_dump(serialize($storage));
echo "===UNSERIALIZE===\n";

$storage2 = unserialize(serialize($storage));

var_dump(count($storage2));

foreach($storage2 as $object)
{
	var_dump($object);
}

var_dump($storage2);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
int(2)
object(TestClass)#%d (4) {
  ["def"]=>
  int(24)
  ["pub"]=>
  int(4)
  ["pro":protected]=>
  int(5)
  ["pri":"TestClass":private]=>
  int(6)
}
object(TestClass)#%d (4) {
  ["def"]=>
  int(24)
  ["pub"]=>
  int(7)
  ["pro":protected]=>
  int(8)
  ["pri":"TestClass":private]=>
  int(9)
}
object(MyStorage)#%d (5) {
  ["def"]=>
  int(24)
  ["pub"]=>
  int(1)
  ["pro":protected]=>
  int(2)
  ["pri":"MyStorage":private]=>
  int(3)
  ["storage":"SplObjectStorage":private]=>
  array(2) {
    ["%s"]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (4) {
        ["def"]=>
        int(24)
        ["pub"]=>
        int(4)
        ["pro":protected]=>
        int(5)
        ["pri":"TestClass":private]=>
        int(6)
      }
      ["inf"]=>
      NULL
    }
    ["%s"]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (4) {
        ["def"]=>
        int(24)
        ["pub"]=>
        int(7)
        ["pro":protected]=>
        int(8)
        ["pri":"TestClass":private]=>
        int(9)
      }
      ["inf"]=>
      NULL
    }
  }
}
string(%d) "%s"
===UNSERIALIZE===
int(2)
object(TestClass)#%d (4) {
  ["def"]=>
  int(24)
  ["pub"]=>
  int(4)
  ["pro":protected]=>
  int(5)
  ["pri":"TestClass":private]=>
  int(6)
}
object(TestClass)#%d (4) {
  ["def"]=>
  int(24)
  ["pub"]=>
  int(7)
  ["pro":protected]=>
  int(8)
  ["pri":"TestClass":private]=>
  int(9)
}
object(MyStorage)#%d (5) {
  ["def"]=>
  int(24)
  ["pub"]=>
  int(1)
  ["pro":protected]=>
  int(2)
  ["pri":"MyStorage":private]=>
  int(3)
  ["storage":"SplObjectStorage":private]=>
  array(2) {
    ["%s"]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (4) {
        ["def"]=>
        int(24)
        ["pub"]=>
        int(4)
        ["pro":protected]=>
        int(5)
        ["pri":"TestClass":private]=>
        int(6)
      }
      ["inf"]=>
      NULL
    }
    ["%s"]=>
    array(2) {
      ["obj"]=>
      object(TestClass)#%d (4) {
        ["def"]=>
        int(24)
        ["pub"]=>
        int(7)
        ["pro":protected]=>
        int(8)
        ["pri":"TestClass":private]=>
        int(9)
      }
      ["inf"]=>
      NULL
    }
  }
}
===DONE===
