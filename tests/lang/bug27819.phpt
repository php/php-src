--TEST--
Bug #27819 (problems returning reference to a reference parameter)
--FILE--
<?php
function &test(&$thing)
{
	$thing += 1 ;
	return $thing ;
}

$a = 6 ;
echo $a . "\n" ;
$a =& test($a);
echo $a. "\n" ;
$a =& test($a);
?>
--EXPECT--
6
7
