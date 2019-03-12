--TEST--
shmop extension error messages
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--SKIPIF--
<?php
	if( substr(PHP_OS, 0, 3) == "WIN") {
		die('skip not for Windows');
	}
	if (!extension_loaded("shmop")) {
		die("skip shmop() extension not available");
	}
?>
--FILE--
<?php
	$hex_shm_id = function(){
		return mt_rand(1338, 9999);
	};

echo PHP_EOL, '## shmop_open function tests ##';
	// warning outputs: invalid flag when the flags length != 1
	var_dump(shmop_open($hex_shm_id(), '', 0644, 1024));

	// warning outputs: invalid access mode
	var_dump(shmop_open($hex_shm_id(), 'b', 0644, 1024));

	// warning outputs: unable to attach or create shared memory segment
	var_dump(shmop_open(null, 'a', 0644, 1024));

	// warning outputs: Shared memory segment size must be greater than zero
	var_dump(shmop_open($hex_shm_id(), "c", 0666, 0));

echo PHP_EOL, '## shmop_read function tests ##';
	// warning outputs: start is out of range
	$shm_id = shmop_open($hex_shm_id(), 'n', 0600, 1024);
	var_dump(shmop_read($shm_id, -10, 0));
	shmop_delete($shm_id);

	// warning outputs: count is out of range
	$shm_id = shmop_open($hex_shm_id(), 'n', 0600, 1024);
	var_dump(shmop_read($shm_id, 0, -10));
	shmop_delete($shm_id);

echo PHP_EOL, '## shmop_write function tests ##';
	// warning outputs: offset out of range
	$shm_id = shmop_open($hex_shm_id(), 'n', 0600, 1024);
	var_dump(shmop_write($shm_id, 'text to try write', -10));
	shmop_delete($shm_id);
?>
--EXPECTF--
## shmop_open function tests ##
Warning: shmop_open():  is not a valid flag in %s on line %d
bool(false)

Warning: shmop_open(): invalid access mode in %s on line %d
bool(false)

Warning: shmop_open(): unable to attach or create shared memory segment 'Invalid argument' in %s on line %d
bool(false)

Warning: shmop_open(): Shared memory segment size must be greater than zero in %s on line %d
bool(false)

## shmop_read function tests ##
Warning: shmop_read(): start is out of range in %s on line %d
bool(false)

Warning: shmop_read(): count is out of range in %s on line %d
bool(false)

## shmop_write function tests ##
Warning: shmop_write(): offset out of range in %s on line %d
bool(false)
