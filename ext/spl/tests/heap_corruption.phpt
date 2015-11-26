--TEST--
SPL: SplHeap - heap corruption via compare exception (with top element deletion)
--CREDITS--
Mike Sullivan <mikesul@php.net>
#TestFest 2009 (London)
--FILE--
<?php

class myHeap extends SplHeap
{
	public $allow_compare = true;
	
	public function compare($v1, $v2)
	{
		if ($this->allow_compare == true)
		{
			if ($v1 > $v2)
			{
				return 1;
			}
			else if ($v1 < $v2)
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			throw new Exception('Compare exception');
		}
	}
}

$heap = new myHeap();
$heap->insert(1);
$heap->insert(2);
$heap->insert(3);
$heap->insert(4);

$heap->allow_compare = false;

var_dump($heap->isCorrupted());

try {
	$heap->extract();
}
catch (Exception $e) {
	echo "Compare Exception: " . $e->getMessage() . PHP_EOL;
}

try {
	$heap->top();
}
catch (Exception $e) {
	echo "Corruption Exception: " . $e->getMessage() . PHP_EOL;
}

var_dump($heap->isCorrupted());
$heap->recoverFromCorruption();
var_dump($heap->isCorrupted());
?>
--EXPECT--
bool(false)
Compare Exception: Compare exception
Corruption Exception: Heap is corrupted, heap properties are no longer ensured.
bool(true)
bool(false)