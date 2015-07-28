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
		return rand(0, 9999);
	};

echo PHP_EOL, '## shmop_open function tests ##';
	// warning outputs: 4 parameters expected
	var_dump($shm_id = shmop_open());

	// warning outputs: invalid flag when the flags length != 1
	var_dump(shmop_open($hex_shm_id(), '', 0644, 1024));

	// warning outputs: invalid access mode
	var_dump(shmop_open($hex_shm_id(), 'b', 0644, 1024));

	// warning outputs: unable to attach or create shared memory segment
	var_dump(shmop_open(null, 'a', 0644, 1024));

	// warning outputs: Shared memory segment size must be greater than zero
	var_dump(shmop_open($hex_shm_id(), "c", 0666, 0));

	// warning outputs: unable to get shared memory segment information
	var_dump(shmop_open($hex_shm_id(), 'n', 0, 1024));

echo PHP_EOL, '## shmop_read function tests ##';
	// warning outputs: 3 parameters expected
	var_dump(shmop_read());

	// warning outputs: start is out of range
	$shm_id = shmop_open($hex_shm_id(), 'n', 0600, 1024);
	var_dump(shmop_read($shm_id, -10, 0));

	// warning outputs: count is out of range
	$shm_id = shmop_open($hex_shm_id(), 'n', 0600, 1024);
	var_dump(shmop_read($shm_id, 0, -10));

echo PHP_EOL, '## shmop_write function tests ##';
	// warning outputs: 3 parameters expected
	var_dump(shmop_write());

	// warning outputs: offset out of range
	$shm_id = shmop_open($hex_shm_id(), 'n', 0600, 1024);
	var_dump(shmop_write($shm_id, 'text to try write', -10));

echo PHP_EOL, '## shmop_size function tests ##';
	// warning outputs: 1 parameter expected
	var_dump(shmop_size());

echo PHP_EOL, '## shmop_delete function tests ##';
	// warning outputs: 1 parameter expected
	var_dump(shmop_delete());

echo PHP_EOL, '## shmop_close function tests ##';
	// warning outputs: 1 parameter expected
	var_dump(shmop_close());
?>
--EXPECTF--
## shmop_open function tests ##
Warning: shmop_open() expects exactly 4 parameters, 0 given in %s on line %d
NULL

Warning: shmop_open():  is not a valid flag in %s on line %d
bool(false)

Warning: shmop_open(): invalid access mode in %s on line %d
bool(false)

Warning: shmop_open(): unable to attach or create shared memory segment 'Invalid argument' in %s on line %d
bool(false)

Warning: shmop_open(): Shared memory segment size must be greater than zero in %s on line %d
bool(false)

Warning: shmop_open(): unable to get shared memory segment information 'Permission denied' in %s on line %d
bool(false)

## shmop_read function tests ##
Warning: shmop_read() expects exactly 3 parameters, 0 given in %s on line %d
NULL

Warning: shmop_read(): start is out of range in %s on line %d
bool(false)

Warning: shmop_read(): count is out of range in %s on line %d
bool(false)

## shmop_write function tests ##
Warning: shmop_write() expects exactly 3 parameters, 0 given in %s on line %d
NULL

Warning: shmop_write(): offset out of range in %s on line %d
bool(false)

## shmop_size function tests ##
Warning: shmop_size() expects exactly 1 parameter, 0 given in %s on line %d
NULL

## shmop_delete function tests ##
Warning: shmop_delete() expects exactly 1 parameter, 0 given in %s on line %d
NULL

## shmop_close function tests ##
Warning: shmop_close() expects exactly 1 parameter, 0 given in %s on line %d
NULL
