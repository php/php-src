--TEST--
Check that SplObjectStorage::unserialize returns NULL when non-string param is passed
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$data_provider = array(
	array(),
	new stdClass(),
);

foreach($data_provider as $input) {

	$s = new SplObjectStorage();

	var_dump($s->unserialize($input));
}

?>
--EXPECTF--
Warning: SplObjectStorage::unserialize() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: SplObjectStorage::unserialize() expects parameter 1 to be string, object given in %s on line %d
NULL
