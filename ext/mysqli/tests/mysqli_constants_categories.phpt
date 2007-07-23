--TEST--
Constants exported by ext/mysqli - checking category - PHP bug not mysqli bug (check from time to time)
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifemb.inc');
require_once('connect.inc');
if (!$TEST_EXPERIMENTAL)
	die("skip This hits a known PHP bug, run the test from time to time and enable it once PHP got fixed");
?>
--FILE--
<?php
	require("connect.inc");
	require("table.inc");

	$constants = get_defined_constants(true);

	foreach ($constants as $group => $consts) {
		foreach ($consts as $name => $value) {
			if (stristr($name, 'mysqli')) {
				if ('mysqli' !== $group)
				printf("found constant '%s' in group '%s'. expecting group 'mysqli'\n", $name, $group);
			}
		}
	}

	print "done!";
?>
--EXPECTF--
done!