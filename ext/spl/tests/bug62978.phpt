--TEST--
Bug #62987 (Assigning to ArrayObject[null][something] overrides all undefined variables)
--FILE--
<?php
$a = new ArrayObject();

$b = array();

$a[null]['hurr'] = 'durr';

var_dump($a['epic_magic']);
var_dump($b['epic_magic']);
var_dump($c['epic_magic']); // Undefined var!!

$d = array();
var_dump($a['epic_magic']); // more magic!
var_dump($d['epic_magic']);

$e = 'srsly?';
var_dump($a['epic_magic']); // srsly.
var_dump(isset($a['epic_magic']));

$fp = fopen(__FILE__, 'r');
var_dump($a[$fp]);

fclose($fp);
?>
--EXPECTF--
Warning: Undefined array key "epic_magic" in %s on line %d
NULL

Warning: Undefined array key "epic_magic" in %s on line %d
NULL

Warning: Undefined variable $c in %s on line %d

Warning: Trying to access array offset on value of type null in %s on line %d
NULL

Warning: Undefined array key "epic_magic" in %s on line %d
NULL

Warning: Undefined array key "epic_magic" in %s on line %d
NULL

Warning: Undefined array key "epic_magic" in %s on line %d
NULL
bool(false)

Warning: Resource ID#%d used as offset, casting to integer (%d) in %sbug62978.php on line %d

Warning: Undefined array key %d in %s on line %d
NULL
