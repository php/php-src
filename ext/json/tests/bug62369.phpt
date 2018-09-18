--TEST--
FR #62369 (Segfault on json_encode(deeply_nested_array)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

$array = array();
for ($i=0; $i < 550; $i++) {
	$array = array($array);
}

json_encode($array, 0, 551);
switch (json_last_error()) {
	case JSON_ERROR_NONE:
		echo 'OK' . PHP_EOL;
	break;
	case JSON_ERROR_DEPTH:
		echo 'ERROR' . PHP_EOL;
	break;
}

json_encode($array, 0, 540);
switch (json_last_error()) {
	case JSON_ERROR_NONE:
		echo 'OK' . PHP_EOL;
	break;
	case JSON_ERROR_DEPTH:
		echo 'ERROR' . PHP_EOL;
	break;
}
--EXPECT--
OK
ERROR
