--TEST--
Bug #34310 (foreach($arr as $c->d => $x) crashes)
--FILE--
<?php

class C
{
	public $d;
}

$c = new C();

$arr = array (1 => 'a', 2 => 'b', 3 => 'c');

// Works fine:
foreach($arr as $x => $c->d)
{
	echo "{$x} => {$c->d}\n";
}

// Crashes:
foreach($arr as $c->d => $x)
{
	echo "{$c->d} => {$x}\n";
}

?>
--EXPECT--
1 => a
2 => b
3 => c
1 => a
2 => b
3 => c
