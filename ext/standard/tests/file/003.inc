<?php

$funcs = array(
	'is_writable',
	'is_readable',
	'is_executable',
	'is_file',
	'file_exists',
);

$filename="";

foreach ($funcs as $test) {
	$bb = $test($filename);
	var_dump($bb);
	clearstatcache();
}

$filename="run-tests.php";

foreach ($funcs as $test) {
	$bb = $test($filename);
	var_dump($bb);
	clearstatcache();
}

?>
