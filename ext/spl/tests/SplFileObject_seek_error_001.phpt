--TEST--
SplFileObject::seek function - test parameters
--FILE--
<?php
$obj = new SplFileObject(__FILE__);
try {
	$obj->seek(-1);
} catch (LogicException $e) {
	echo($e->getMessage());
}
?>
--EXPECT--
Can't seek file %s to negative line %s
