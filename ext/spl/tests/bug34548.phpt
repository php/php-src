--TEST--
Bug #34548 (Method append() in class extended from ArrayObject crashes PHP)
--FILE--
<?php

class Collection extends ArrayObject
{
	public function add($dataArray)
	{
		foreach($dataArray as $value) $this->append($value);
	}

	public function offsetSet($index, $value)
	{
		parent::offsetSet($index, $value);
	}
}

$data1=array('one', 'two', 'three');
$data2=array('four', 'five');

$foo=new Collection($data1);
$foo->add($data2);

print_r($foo->getArrayCopy());

echo "Done\n";
?>
--EXPECT--
Array
(
    [0] => one
    [1] => two
    [2] => three
    [3] => four
    [4] => five
)
Done
