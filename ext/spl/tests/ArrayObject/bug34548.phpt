--TEST--
Bug #34548 (Method append() in class extended from ArrayObject crashes PHP)
--FILE--
<?php

class Collection extends ArrayObject
{
    public function add($dataArray)
    {
        foreach ($dataArray as $value) {
            $this->append($value);
        }
    }
}

$data1 = ['one', 'two', 'three'];
$data2 = ['four', 'five'];

$foo = new Collection($data1);
$foo->add($data2);

var_dump($foo->getArrayCopy());

echo "Done\n";
?>
--EXPECT--
array(5) {
  [0]=>
  string(3) "one"
  [1]=>
  string(3) "two"
  [2]=>
  string(5) "three"
  [3]=>
  string(4) "four"
  [4]=>
  string(4) "five"
}
Done
