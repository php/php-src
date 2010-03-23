--TEST--
Bug #28800 (Incorrect string to number conversion for strings starting with 'inf')
--FILE--
<?php
	$strings = array('into', 'info', 'inf', 'infinity', 'infin', 'inflammable');
	foreach ($strings as $v) {
		echo ($v+0)."\n";
	}
?>
--EXPECT--
0
0
0
0
0
0

