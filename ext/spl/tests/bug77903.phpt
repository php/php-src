--TEST--
Bug #77903: ArrayIterator stops iterating after offsetSet call
--FILE--
<?php
$a = new ArrayIterator();
$a->rewind();
var_dump($a->valid()); // false
var_dump($a->current()); // null
$a->offsetSet(1,1);
var_dump($a->valid()); // true
var_dump($a->current()); // 1
$a->next();
var_dump($a->valid()); // false
var_dump($a->current()); // null
$a->offsetSet(4,4);
var_dump($a->valid()); // true
var_dump($a->current()); // 4
$a->next();
var_dump($a->valid()); // false
var_dump($a->current()); // null
$a->next();
var_dump($a->valid()); // false
var_dump($a->current()); // null
$a->offsetSet(2,2);
var_dump($a->valid()); // true
var_dump($a->current()); // 2
$a->next();
var_dump($a->valid()); // false
var_dump($a->current()); // null
$a->next();
var_dump($a->valid()); // false
var_dump($a->current()); // null
?>
--EXPECT--
bool(false)
NULL
bool(true)
int(1)
bool(false)
NULL
bool(true)
int(4)
bool(false)
NULL
bool(false)
NULL
bool(true)
int(2)
bool(false)
NULL
bool(false)
NULL
