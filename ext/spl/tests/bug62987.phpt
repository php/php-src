--TEST--
Bug #62987 (Assigning to ArrayObject[null][something] overrides all undefined variables)
--FILE--
<?php

$a = new ArrayObject();

$b = array();

$a[null]['hurr'] = 'durr';

$d = array();
var_dump($d['non-exist-key']);

$undefine = 'will null be overwrited?';

var_dump($what_ever);
var_dump($a['non-exist-key']);
var_dump($d['non-exist-key']);

var_dump($a);
?>
===DONE===
--EXPECTF--
Notice: Undefined index: non-exist-key in %sbug62987.php on line %d
NULL

Notice: Undefined variable: what_ever in %sbug62987.php on line %d
NULL

Notice: Undefined index:  non-exist-key in %sbug62987.php on line %d
NULL

Notice: Undefined index: non-exist-key in %sbug62987.php on line %d
NULL
object(ArrayObject)#%d (%d) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    [""]=>
    array(1) {
      ["hurr"]=>
      string(4) "durr"
    }
  }
}
===DONE===
