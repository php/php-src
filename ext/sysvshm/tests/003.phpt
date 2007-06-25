--TEST--
shm_detach() tests
--SKIPIF--
<?php if (!extension_loaded("sysvshm")) print "skip"; ?>
--FILE--
<?php

$key = ftok(dirname(__FILE__)."/003.phpt", 'q');

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
--CLEAN--
<?php

$key = ftok(dirname(__FILE__)."/003.phpt", 'q');
$s = shm_attach($key);
shm_remove($s);

?>
--EXPECTF--	
Warning: Wrong parameter count for shm_detach() in %s on line %d
NULL

Warning: Wrong parameter count for shm_detach() in %s on line %d
NULL
bool(true)

Warning: shm_detach(): The parameter is not a valid shm_identifier in %s on line %d
bool(false)

Warning: shm_remove(): The parameter is not a valid shm_identifier in %s on line %d

Warning: shm_detach(): The parameter is not a valid shm_identifier in %s on line %d
bool(false)

Warning: shm_detach(): The parameter is not a valid shm_identifier in %s on line %d
bool(false)

Warning: shm_detach(): The parameter is not a valid shm_identifier in %s on line %d
bool(false)
Done
