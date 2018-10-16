--TEST--
Check that SplPriorityQueue::extract generate a warning and returns NULL when param passed
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

	$h = new SplPriorityQueue();

	var_dump($h->extract($input));
}

?>
--EXPECTF--
Warning: SplPriorityQueue::extract() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: SplPriorityQueue::extract() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: SplPriorityQueue::extract() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: SplPriorityQueue::extract() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: SplPriorityQueue::extract() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: SplPriorityQueue::extract() expects exactly 0 parameters, 1 given in %s on line %d
NULL

Warning: SplPriorityQueue::extract() expects exactly 0 parameters, 1 given in %s on line %d
NULL
