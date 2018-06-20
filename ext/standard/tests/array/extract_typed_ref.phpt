--TEST--
extract() into typed references must respect their type
--FILE--
<?php
  
class Test {
	public int $i = 0;
	public string $s = "";
}

$test = new Test;
$i =& $test->i;
$s =& $test->s;
try {
	extract(['i' => 'foo', 's' => 42]);
} catch (TypeError $e) { var_dump($e->getMessage()); }
var_dump($test->i, $test->s);

?>
--EXPECT--
string(45) "Cannot assign string to reference of type int"
int(0)
string(2) "42"
