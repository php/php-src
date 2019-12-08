--TEST--
SplFileObject::seek function - test parameters
--FILE--
<?php
$obj = new SplFileObject(__FILE__);
try {
	$obj->seek(-1);
} catch (\ValueError $e) {
	echo($e->getMessage());
}
?>
--EXPECTF--
Can't seek file %s to negative line
