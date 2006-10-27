--TEST--
shm_detach() tests
--SKIPIF--
<?php if (!extension_loaded("sysvshm")) print "skip"; ?>
--FILE--
<?php

$key = ftok(__FILE__, 'q');

var_dump(shm_detach());
var_dump(shm_detach(1,1));

$s = shm_attach($key);

var_dump(shm_detach($s));
var_dump(shm_detach($s));
shm_remove($s);

var_dump(shm_detach(0));
var_dump(shm_detach(1));
var_dump(shm_detach(-1));

echo "Done\n";
?>
--EXPECTF--
Warning: shm_detach() expects exactly 1 parameter, 0 given in %s003.php on line %d
bool(false)

Warning: shm_detach() expects exactly 1 parameter, 2 given in %s003.php on line %d
bool(false)
bool(true)

Warning: shm_detach(): %d is not a valid sysvshm resource in %s003.php on line %d
bool(false)

Warning: shm_remove(): %d is not a valid sysvshm resource in %s003.php on line %d

Warning: shm_detach() expects parameter 1 to be resource, integer given in %s003.php on line %d
bool(false)

Warning: shm_detach() expects parameter 1 to be resource, integer given in %s003.php on line %d
bool(false)

Warning: shm_detach() expects parameter 1 to be resource, integer given in %s003.php on line %d
bool(false)
Done
