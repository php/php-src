--TEST--
Bug #34064 (arr[] as param to function in class gives invalid opcode)
--FILE--
<?php
class XmlTest {

    function test_ref(&$test)
    {
    	$test = "ok";
    }

    function test($test)
    {
    }

    function run()
    {
        $ar = array();
        $this->test_ref($ar[]);
        var_dump($ar);
        $this->test($ar[]);
    }
}

$o = new XmlTest();
$o->run();
?>
--EXPECTF--
array(1) {
  [0]=>
  string(2) "ok"
}

Fatal error: Uncaught Error: Cannot use [] for reading in %sbug34064.php:18
Stack trace:
#0 %s(%d): XmlTest->run()
#1 {main}
  thrown in %sbug34064.php on line 18
