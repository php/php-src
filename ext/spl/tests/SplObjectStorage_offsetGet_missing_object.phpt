--TEST--
Check that SplObjectStorage::offsetGet throws exception when non-existing object is requested
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$s = new SplObjectStorage();
$o1 = new stdClass();

try {
	$s->offsetGet($o1);
} catch (UnexpectedValueException $e) {
	echo $e->getMessage();
}

?>
--EXPECT--
Object not found
