--TEST--
bug #44859 (incorrect result with NTFS ACL permissions, is_writable)
--SKIPIF--
<?php
include_once __DIR__ . '/common.inc';
skipif();
?>
--FILE--
<?php
$uniqueBasePath = substr(__FILE__, 0, strrpos(__FILE__, '.'));
include_once __DIR__ . '/common.inc';
fix_acls();

$iteration = array(
	PHPT_ACL_READ => false,
	PHPT_ACL_NONE => false,
	PHPT_ACL_WRITE => true,
	PHPT_ACL_WRITE|PHPT_ACL_READ => true,
);

echo "Testing file:\n";
$i = 1;
$path = $uniqueBasePath . '_file.txt';
foreach ($iteration as $perms => $exp) {
	create_file($path, $perms);
	clearstatcache(true, $path);
	echo 'Iteration #' . $i++ . ': ';
	if (is_writable($path) == $exp) {
		echo "passed.\n";
	} else {
		var_dump(is_writable($path), $exp);
		echo "failed.\n";
	}
	delete_file($path);
}

echo "Testing directory:\n";
$path = $uniqueBasePath . '_dir';
$i = 1;
foreach ($iteration as $perms => $exp) {
	create_dir($path, $perms);
	clearstatcache(true, $path);
	echo 'Iteration #' . $i++ . ': ';
	if (is_writable($path) == $exp) {
		echo "passed.\n";
	} else {
		var_dump(is_writable($path), $exp);
		echo "failed.\n";
	}
	delete_dir($path);
}

?>
--EXPECT--
Testing file:
Iteration #1: passed.
Iteration #2: passed.
Iteration #3: passed.
Iteration #4: passed.
Testing directory:
Iteration #1: passed.
Iteration #2: passed.
Iteration #3: passed.
Iteration #4: passed.
