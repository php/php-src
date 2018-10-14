--TEST--
shm_get_var() tests
--SKIPIF--
<?php
if (!extension_loaded("sysvshm")){ print 'skip'; }
if (!function_exists('ftok')){ print 'skip'; }
?>
--FILE--
<?php

$key = ftok(__FILE__, 't');
$s = shm_attach($key, 1024);

shm_put_var($s, -1, "test string");
shm_put_var($s, 0, new stdclass);
shm_put_var($s, 1, array(1,2,3));
shm_put_var($s, 2, false);
shm_put_var($s, 3, null);

var_dump(shm_get_var());

var_dump(shm_get_var(-1, -1));

var_dump(shm_get_var($s, 1000));
var_dump(shm_get_var($s, -10000));

var_dump(shm_get_var($s, array()));
var_dump(shm_get_var($s, -1));
var_dump(shm_get_var($s, 0));
var_dump(shm_get_var($s, 1));
var_dump(shm_get_var($s, 2));
var_dump(shm_get_var($s, 3));

shm_put_var($s, 3, "test");
shm_put_var($s, 3, 1);
shm_put_var($s, 3, null);

var_dump(shm_get_var($s, 3));
shm_remove($s);

echo "Done\n";
?>
--EXPECTF--
Warning: shm_get_var() expects exactly 2 parameters, 0 given in %s005.php on line %d
NULL

Warning: shm_get_var() expects parameter 1 to be resource, integer given in %s005.php on line %d
NULL

Warning: shm_get_var(): variable key 1000 doesn't exist in %s005.php on line %d
bool(false)

Warning: shm_get_var(): variable key -10000 doesn't exist in %s005.php on line %d
bool(false)

Warning: shm_get_var() expects parameter 2 to be integer, array given in %s005.php on line %d
NULL
string(11) "test string"
object(stdClass)#%d (0) {
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
bool(false)
NULL
NULL
Done
