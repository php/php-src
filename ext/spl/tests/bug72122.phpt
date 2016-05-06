--TEST--
Bug #72122 (IteratorIterator breaks '@' error suppression)
--FILE--
<?php
class CustomIterator implements IteratorAggregate {
	public function getIterator() {
		@unlink('/missing/file.txt');
		return new ArrayIterator(array('item'));
	}
}

$obj = new CustomIterator;
$iterator = new IteratorIterator($obj);
echo get_class($iterator);
?>
--EXPECT--
IteratorIterator