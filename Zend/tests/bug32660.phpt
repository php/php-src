--TEST--
Bug #32660 (Assignment by reference causes crash when field access is overloaded (__get))
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
Notice: Indirect modification of overloaded property A::$whatever has no effect in %sbug32660.php on line 20
A Object
(
    [q] => 3
)

Notice: Indirect modification of overloaded property A::$whatever has no effect in %sbug32660.php on line 23

Fatal error: Uncaught Error: Cannot assign by reference to overloaded object in %sbug32660.php:23
Stack trace:
#0 {main}
  thrown in %sbug32660.php on line 23
