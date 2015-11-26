--TEST--
Check that SplHeap::count generate a warning and returns NULL when param passed
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$data_provider = array(
    new stdClass,
	array(),
	true,
	"string",
	12345,
	1.2345,
	NULL
);

foreach($data_provider as $input) {

	$h = new SplMaxHeap();

	var_dump($h->count($input));
}

?>
--EXPECTF--
Warning: SplHeap::count() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: SplHeap::count() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: SplHeap::count() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: SplHeap::count() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: SplHeap::count() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: SplHeap::count() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: SplHeap::count() expects exactly 0 parameters, 1 given in %s on line %d
NULL

