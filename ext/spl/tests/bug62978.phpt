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
--EXPECTF--
Notice: Undefined index: epic_magic in %sbug62978.php on line %d
NULL

Notice: Undefined index: epic_magic in %sbug62978.php on line %d
NULL

Notice: Undefined variable: c in %sbug62978.php on line %d
NULL

Notice: Undefined index: epic_magic in %sbug62978.php on line %d
NULL

Notice: Undefined index: epic_magic in %sbug62978.php on line %d
NULL

Notice: Undefined index: epic_magic in %sbug62978.php on line %d
NULL
bool(false)

Notice: Resource ID#%d used as offset, casting to integer (%d) in %sbug62978.php on line %d

Notice: Undefined offset: %d in %sbug62978.php on line %d
NULL
