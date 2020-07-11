--TEST--
SPL: ArrayObject::count() and ArrayIterator::count() basic functionality.
--FILE--
==ArrayObject==
<?php
class C extends ArrayObject {
  function count() {
    return 99;
  }
}

$c = new C;
$ao = new ArrayObject;

var_dump(count($c), count($ao));

$c[] = 'a';
$ao[] = 'a';
var_dump(count($c), count($ao));

$c[] = 'b';
$ao[] = 'b';
var_dump(count($c), count($ao));

unset($c[0]);
unset($ao[0]);
var_dump($c->count(), $ao->count());
?>
==ArrayIterator==
<?php
class D extends ArrayIterator {
  function count() {
    return 99;
  }
}

$c = new D;
$ao = new ArrayIterator;

var_dump(count($c), count($ao));

$c[] = 'a';
$ao[] = 'a';
var_dump(count($c), count($ao));

$c[] = 'b';
$ao[] = 'b';
var_dump(count($c), count($ao));

unset($c[0]);
unset($ao[0]);
var_dump($c->count(), $ao->count());
?>
--EXPECT--
==ArrayObject==
int(99)
int(0)
int(99)
int(1)
int(99)
int(2)
int(99)
int(1)
==ArrayIterator==
int(99)
int(0)
int(99)
int(1)
int(99)
int(2)
int(99)
int(1)
