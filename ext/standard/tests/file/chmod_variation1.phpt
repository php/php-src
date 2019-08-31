--TEST--
chmod() on a directory
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip non-windows only test');
}
?>
--FILE--
<?php

define("PERMISSIONS_MASK", 0777);

$dirname = __DIR__ . "/" . basename(__FILE__, ".php") . "testdir";
mkdir($dirname);

for ($perms_to_set = 0777; $perms_to_set >= 0; $perms_to_set--) {
	chmod($dirname, $perms_to_set);
	$set_perms = (fileperms($dirname) & PERMISSIONS_MASK);
	clearstatcache();
	if ($set_perms != $perms_to_set) {
		printf("Error: %o does not match %o\n", $set_perms, $perms_to_set);
	}
}

var_dump(chmod($dirname, 0777));
rmdir($dirname);

echo "done";

?>
--EXPECT--
bool(true)
done
