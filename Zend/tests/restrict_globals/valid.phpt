--TEST--
Supported operations on $GLOBALS
--FILE--
<?php

function test() {
    var_dump($GLOBALS['x']);
    $GLOBALS['x'] = 1;
    var_dump($GLOBALS['x']);
    $GLOBALS['x']++;
    var_dump($GLOBALS['x']);
    $GLOBALS['x'] += 2;
    var_dump($GLOBALS['x']);
    unset($GLOBALS['y']);
    var_dump(isset($GLOBALS['x']));
    var_dump(isset($GLOBALS['y']));
    $GLOBALS['z'][] = 1;
}

$y = 1;
test();
var_dump($x, $y, $z);

$ref = 1;
$GLOBALS['z'] =& $ref;
$ref++;
var_dump($z);

$x = 1;
$ref2 =& $GLOBALS['x'];
$ref2++;
var_dump($x);

?>
--EXPECTF--
Warning: Undefined global variable $x in %s on line %d
NULL
int(1)
int(2)
int(4)
bool(true)
bool(false)

Warning: Undefined variable $y in %s on line %d
int(4)
NULL
array(1) {
  [0]=>
  int(1)
}
int(2)
int(2)
