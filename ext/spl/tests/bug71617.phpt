--TEST--
Bug #71617: private properties lost when unserializing ArrayObject
--FILE--
<?php

class Test extends ArrayObject
{

	private $name = null;

	public function __construct(array $input)
	{
		parent::__construct($input, ArrayObject::ARRAY_AS_PROPS);
	}

	public function setName($name)
	{
		$this->name = $name;
		return $this;
	}

	public function getName()
	{
		return $this->name;
	}
}

$test = new Test(['a' => 'a', 'b' => 'b']);
$test->setName('ok');

$ser = serialize($test);
$unSer = unserialize($ser);

var_dump($unSer->getName());
var_dump($unSer);

?>
--EXPECT--
string(2) "ok"
object(Test)#2 (2) {
  ["name":"Test":private]=>
  string(2) "ok"
  ["storage":"ArrayObject":private]=>
  array(2) {
    ["a"]=>
    string(1) "a"
    ["b"]=>
    string(1) "b"
  }
}
