--TEST--
SplFileObject::seek function - test parameters
--FILE--
<?php
$obj = New SplFileObject(__FILE__);
$obj->seek(1,2);
$obj->seek();
try {
	$obj->seek(-1);
} catch (LogicException $e) {
	echo($e->getMessage());
}
?>
--EXPECTF--
Warning: SplFileObject::seek() expects exactly 1 parameter, 2 given in %s

Warning: SplFileObject::seek() expects exactly 1 parameter, 0 given in %s
Can't seek file %s to negative line %s
