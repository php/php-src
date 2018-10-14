--TEST--
shm_detach() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
if (!function_exists('ftok')){ print 'skip'; }
?>
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
Warning: shm_detach() expects exactly 1 parameter, 0 given in %s003.php on line %d
NULL

Warning: shm_detach() expects exactly 1 parameter, 2 given in %s003.php on line %d
NULL
bool(true)

Warning: shm_detach(): supplied resource is not a valid sysvshm resource in %s003.php on line %d
bool(false)

Warning: shm_remove(): supplied resource is not a valid sysvshm resource in %s003.php on line %d

Warning: shm_detach() expects parameter 1 to be resource, int given in %s003.php on line %d
NULL

Warning: shm_detach() expects parameter 1 to be resource, int given in %s003.php on line %d
NULL

Warning: shm_detach() expects parameter 1 to be resource, int given in %s003.php on line %d
NULL
Done
