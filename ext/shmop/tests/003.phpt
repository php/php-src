--TEST--
shmop extension error messages (non-root)
--SKIPIF--
<?php
	if( substr(PHP_OS, 0, 3) == "WIN") {
		die('skip not for Windows');
	}
	if (!extension_loaded("shmop")) {
		die("skip shmop extension not available");
	}
	if (!extension_loaded("posix")) {
		die("skip posix extension not available");
	}
	if (!posix_geteuid()) {
		die("skip cannot run test as root");
	}
?>
--FILE--
<?php
	$hex_shm_id = function(){
		return mt_rand(1338, 9999);
	};

echo '## shmop_open function tests ##', PHP_EOL;
	// warning outputs: unable to get shared memory segment information
	var_dump(shmop_open($hex_shm_id(), 'n', 0, 1024));

?>
--EXPECTF--
## shmop_open function tests ##

Warning: shmop_open(): unable to get shared memory segment information 'Permission denied' in %s on line %d
bool(false)
