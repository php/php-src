--TEST--
SPL: SplHeap::current - get current value from empty heap
--CREDITS--
Mike Sullivan <mikesul@php.net>
#TestFest 2009 (London)
--FILE--
<?php

class myHeap extends SplHeap
{
	public function compare($v1, $v2)
	{
		throw new Exception('');
	}
}

$heap = new myHeap();
var_dump($heap->current());

?>
--EXPECT--
NULL