--TEST--
Test DateTimeInterval readonly properties
--FILE--
<?php

$i = new DateTimeInterval("P1D");

try {
    $i->from_string = true;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $i->y = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $i->m = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $i->d = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $i->h = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $i->i = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $i->s = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $i->f = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $i->invert = true;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $i->days = 1;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump($i);

?>
--EXPECTF--
Cannot modify readonly property DateTimeInterval::$from_string
Cannot modify readonly property DateTimeInterval::$y
Cannot modify readonly property DateTimeInterval::$m
Cannot modify readonly property DateTimeInterval::$d
Cannot modify readonly property DateTimeInterval::$h
Cannot modify readonly property DateTimeInterval::$i
Cannot modify readonly property DateTimeInterval::$s
Cannot modify readonly property DateTimeInterval::$f
Cannot modify readonly property DateTimeInterval::$invert
Cannot modify readonly property DateTimeInterval::$days
object(DateTimeInterval)#%d (%d) {
  ["from_string"]=>
  bool(false)
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(1)
  ["h"]=>
  int(0)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
}
