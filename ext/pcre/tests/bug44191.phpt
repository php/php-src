--TEST--
Bug #44191 (preg_grep messes up array index)
--FILE--
<?php

$array = range(1, 10);

preg_grep('/asdf/', $array);

foreach ($array as $k => $v) {
	print $k;
}

?>
--EXPECT--
0123456789
