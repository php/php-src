--TEST--
shm_attach() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
if (!function_exists('ftok')){ print 'skip'; }
?>
--FILE--
<?php

$key = ftok(__FILE__, 't');

var_dump(shm_attach(-1, 0));
var_dump(shm_attach(0, -1));
var_dump(shm_attach(123, -1));
var_dump(shm_attach($key, -1));
var_dump(shm_attach($key, 0));

var_dump($s = shm_attach($key, 1024));
shm_remove($s);
var_dump($s = shm_attach($key, 1024));
shm_remove($s);
var_dump($s = shm_attach($key, 1024, 0666));
shm_remove($s);

var_dump($s = shm_attach($key, 1024));
shm_remove($s);
var_dump($s = shm_attach($key));
shm_remove($s);

echo "Done\n";
?>
--EXPECTF--
Warning: shm_attach(): Segment size must be greater than zero in %s on line %d
bool(false)

Warning: shm_attach(): Segment size must be greater than zero in %s on line %d
bool(false)

Warning: shm_attach(): Segment size must be greater than zero in %s on line %d
bool(false)

Warning: shm_attach(): Segment size must be greater than zero in %s on line %d
bool(false)

Warning: shm_attach(): Segment size must be greater than zero in %s on line %d
bool(false)
resource(%d) of type (sysvshm)
resource(%d) of type (sysvshm)
resource(%d) of type (sysvshm)
resource(%d) of type (sysvshm)
resource(%d) of type (sysvshm)
Done
