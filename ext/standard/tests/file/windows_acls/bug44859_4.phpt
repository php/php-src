--TEST--
bug #44859 (incorrect result with NTFS ACL permissions, is_readable)
--CREDITS--
Venkat Raman Don
--SKIPIF--
<?php
include_once __DIR__ . '/common.inc';
skipif();
?>
--FILE--
<?php
$uniqueBaseName = basename(substr(__FILE__, 0, strrpos(__FILE__, '.')));
include_once __DIR__ . '/common.inc';
fix_acls();

$iteration = array(
	PHPT_ACL_READ => true,
	PHPT_ACL_NONE => false,
	PHPT_ACL_WRITE => false,
	PHPT_ACL_WRITE|PHPT_ACL_READ => true,
);

echo "Testing file with relative path:\n";
$i = 1;
$path = './' . $uniqueBaseName . '_file.txt';
foreach ($iteration as $perms => $exp) {
	create_file($path, $perms);
	clearstatcache(true, $path);
	echo 'Iteration #' . $i++ . ': ';
	if (is_readable($path) == $exp) {
		echo "passed.\n";
	} else {
		var_dump(is_readable($path), $exp);
		echo "failed.\n";
	}
	delete_file($path);
}

echo "Testing directory with relative path:\n";
$path = $uniqueBaseName . '_dir';
$i = 1;
foreach ($iteration as $perms => $exp) {
	create_dir($path, $perms);
	clearstatcache(true, $path);
	echo 'Iteration #' . $i++ . ': ';
	if (is_readable($path) == $exp) {
		echo "passed.\n";
	} else {
		var_dump(is_readable($path), $exp);
		echo "failed.\n";
	}
	delete_dir($path);
}

?>
--EXPECT--
Testing file with relative path:
Iteration #1: passed.
Iteration #2: passed.
Iteration #3: passed.
Iteration #4: passed.
Testing directory with relative path:
Iteration #1: passed.
Iteration #2: passed.
Iteration #3: passed.
Iteration #4: passed.
