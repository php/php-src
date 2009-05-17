--TEST--
bug #44859 (incorrect result with NTFS ACL permissions, is_executable)
--SKIPIF--
<?php 
include_once __DIR__ . '/common.inc';
skipif();
?>
--FILE--
<?php
include_once __DIR__ . '/common.inc';

$iteration = array(
	'tiny.exe' => true,
	//'tiny.bat' => true, To be fixed in _access
	__FILE__ => false
);

$i = 1;
$path = __DIR__;

foreach ($iteration as $file => $exp) {
	$path = __DIR__ . '/' . $file;
	echo 'Iteration #' . $i++ . ': ';
	if (is_executable($path) == $exp) {
		echo "passed.\n";
	} else {
		var_dump(is_executable($path), $exp);
		echo "failed.\n";
	}
}


?>
--EXPECT--
Iteration #1: passed.
Iteration #2: passed.
