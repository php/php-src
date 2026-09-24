--TEST--
shm_get_var() leaks if variable is corrupted
--EXTENSIONS--
sysvshm
ffi
--INI--
ffi.enable=1
--SKIPIF--
<?php
if (!function_exists('ftok')) die('skip needs ftok');
if (PHP_OS_FAMILY !== 'Linux') die('skip only for decent operating systems');
?>
--FILE--
<?php

$key = ftok(__FILE__, 't');
$s = shm_attach($key, 128);

shm_put_var($s, 0, [1, 2]);

$ffi = FFI::cdef(<<<CODE
int shmget(int, size_t, int);
char *shmat(int, const void *, int);
CODE);
$ptr = $ffi->shmat($ffi->shmget($key, 0, 0), $ffi->new('void *'), 0);

// Locate the serialized data, its offset depends on the zend_long and size_t widths
$offset = strpos(FFI::string($ptr, 128), 'a:2:{');
$ptr[$offset + 13] = 0; // Corrupt first byte of second element of serialized data

var_dump(shm_get_var($s, 0));

shm_remove($s);

?>
--EXPECTF--
Warning: shm_get_var(): Variable data in shared memory is corrupted in %s on line %d
bool(false)
