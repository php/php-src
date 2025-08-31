--TEST--
Bug #36743 (In a class extending XMLReader array properties are not writable)
--EXTENSIONS--
xmlreader
--FILE--
<?php

class Test extends XMLReader
{
    private $testArr = array();
    public function __construct()
    {
        $this->testArr[] = 1;
        var_dump($this->testArr);
    }
}

$t = new test;

echo "Done\n";
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
Done
