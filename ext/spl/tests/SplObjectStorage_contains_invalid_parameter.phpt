--TEST--
Check that SplObjectStorage::contains generate a warning and returns NULL when passed non-object param
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

	var_dump($s->contains($input));
}

?>
--EXPECTF--
Warning: SplObjectStorage::contains() expects parameter 1 to be object, array given in %s on line %d
NULL

Warning: SplObjectStorage::contains() expects parameter 1 to be object, boolean given in %s on line %d
NULL

Warning: SplObjectStorage::contains() expects parameter 1 to be object, %unicode_string_optional% given in %s on line %d
NULL

Warning: SplObjectStorage::contains() expects parameter 1 to be object, integer given in %s on line %d
NULL

Warning: SplObjectStorage::contains() expects parameter 1 to be object, double given in %s on line %d
NULL

Warning: SplObjectStorage::contains() expects parameter 1 to be object, null given in %s on line %d
NULL

