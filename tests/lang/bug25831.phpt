--TEST--
Bug #25831 (pass-by-reference malfunction on overloaded method call)
--FILE--
<?php 
class no_problem  {
	function pass_by_reference(&$ref)	{
		$ref = "Pass by reference works";
	}
}

class problem  { 
	function pass_by_reference(&$ref)	{
		$ref = "Pass by reference works";
	}
	// simple dummy call implementation..
    function __call($method,$params,&$return) {
		if ($method == get_class($this)) {
			return true;	
        }
		return false; // not found!
	}
}

overload('problem');

$good = &new no_problem;
$bad = &new problem;

$message = "Pass by reference does not work!";
$good->pass_by_reference($message);
print "$message\n";

$message = "Pass by reference does not work!";
$bad->pass_by_reference($message);
print "$message\n";

?>
--EXPECT--
Pass by reference works
Pass by reference works
