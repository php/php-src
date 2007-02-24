--TEST--
shm_attach() tests
--SKIPIF--
<?php if (!extension_loaded("sysvshm")) print "skip"; ?>
--FILE--
<?php

$key = ftok(__FILE__, 't');

var_dump(shm_attach());
var_dump(shm_attach(1,2,3,4));

var_dump(shm_attach(-1, 0));
var_dump(shm_attach(0, -1));
var_dump(shm_attach(123, -1));
var_dump($s = shm_attach($key, -1));
shm_remove($s);
var_dump($s = shm_attach($key, 0));
shm_remove($s);

var_dump($s = shm_attach($key, 1024));
shm_remove($key);
var_dump($s = shm_attach($key, 1024));
shm_remove($s);
var_dump(shm_attach($key, 1024, 0666));
shm_remove($s);

var_dump($s = shm_attach($key, 1024));
shm_remove($s);
var_dump($s = shm_attach($key));
shm_remove($s);

echo "Done\n";
?>
--EXPECTF--	
Warning: Wrong parameter count for shm_attach() in %s/sysvshm/tests/002.php on line %d
NULL

Warning: Wrong parameter count for shm_attach() in %s/sysvshm/tests/002.php on line %d
NULL

Warning: shm_attach(): Segment size must be greater then zero. in %s/sysvshm/tests/002.php on line %d
bool(false)

Warning: shm_attach(): Segment size must be greater then zero. in %s/sysvshm/tests/002.php on line %d
bool(false)

Warning: shm_attach(): Segment size must be greater then zero. in %s/sysvshm/tests/002.php on line %d
bool(false)

Warning: shm_attach(): Segment size must be greater then zero. in %s/sysvshm/tests/002.php on line %d
bool(false)

Warning: shm_remove(): The parameter is not a valid shm_identifier in %s/sysvshm/tests/002.php on line %d

Warning: shm_attach(): Segment size must be greater then zero. in %s/sysvshm/tests/002.php on line %d
bool(false)

Warning: shm_remove(): The parameter is not a valid shm_identifier in %s/sysvshm/tests/002.php on line %d
int(%d)

Warning: shm_remove(): The parameter is not a valid shm_identifier in %s/sysvshm/tests/002.php on line %d
int(%d)
int(%d)
int(%d)
int(%d)
Done