--TEST--
Bug #32660 Assignment by reference causes crash when field access is overloaded (__get) 
--FILE--
<?php
class A
{
	public $q;

	function __construct()
	{
		$this->q = 3;//array();
	}

	function __get($name)
	{
		return $this->q;
	}
}

$a = new A;

$b = "short";
$c =& $a->whatever;
$c = "long";
print_r($a);
$a->whatever =& $b;
$b = "much longer";
print_r($a);
?>
--EXPECTF--
A Object
(
    [q] => long
)

Fatal error: Cannot assign by reference to overloaded object in %sbug32660.php on line 23
