--TEST--
Bug #30266 (Invalid opcode 137/1/8) and array_walk
--FILE--
<?php
class testc
{
	public $b = "c";

	function crash($val)
	{
		$this->b = $val;
		throw new Exception("Error");
	}
}

$fruits = array ("d"=>"lemon", "a"=>"orange", "b"=>"banana", "c"=>"apple");

$myobj = new testc();

function test($item2, $key, $userd)
{
	$userd->crash($item2);
}

try
{
	array_walk($fruits, 'test', $myobj);
}
catch(Exception $e)
{
	echo "Caught: " . $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECT--
Caught: Error
===DONE===
