--TEST--
Check that SplObjectStorage::addAll generate a warning and returns NULL when passed non-object param
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$data_provider = array(
	array(),
	true,
	"string",
	12345,
	1.2345,
	NULL
);

foreach($data_provider as $input) {

	$s = new SplObjectStorage();

	var_dump($s->addAll($input));
}

?>
--EXPECTF--
Warning: SplObjectStorage::addAll() expects parameter 1 to be SplObjectStorage, array given in %s on line %d
NULL

Warning: SplObjectStorage::addAll() expects parameter 1 to be SplObjectStorage, boolean given in %s on line %d
NULL

Warning: SplObjectStorage::addAll() expects parameter 1 to be SplObjectStorage, string given in %s on line %d
NULL

Warning: SplObjectStorage::addAll() expects parameter 1 to be SplObjectStorage, integer given in %s on line %d
NULL

Warning: SplObjectStorage::addAll() expects parameter 1 to be SplObjectStorage, float given in %s on line %d
NULL

Warning: SplObjectStorage::addAll() expects parameter 1 to be SplObjectStorage, null given in %s on line %d
NULL
