--TEST--
Bug #61527 (Recursive/ArrayIterator gives misleading notice when array empty or moved to the end)
--FILE--
<?php
$ao = new ArrayObject(array());
$ai = $ao->getIterator();

/* testing empty array, should no notice at all */
$ai->next();
var_dump($ai->key());
var_dump($ai->current());

/* testing array changing */
$ao2 = new ArrayObject(array(1 => 1, 2, 3, 4, 5));
$ai2 = $ao2->getIterator();

$ao2->offsetUnset($ai2->key());
$ai2->next();

/* now point to 2 */
$ao2->offsetUnset($ai2->key());
var_dump($ai2->key());

/* now point to 3 */
$ao2->offsetUnset($ai2->key());
var_dump($ai2->current());

$ai2->next();
var_dump($ai2->key());
var_dump($ai2->current());

/* should be at the end and no notice */
$ai2->next();
var_dump($ai2->key());
var_dump($ai2->current());

$ai2->rewind();
$ai2->next();
$ai2->next();
/* should reached the end */
var_dump($ai2->next());
var_dump($ai2->key());

/* testing RecursiveArrayIterator */
$ao3 = new ArrayObject(array(), 0, 'RecursiveArrayIterator');
$ai3 = $ao3->getIterator();

var_dump($ai3->getChildren());

$ao4 = new ArrayObject(array(1, 2), 0, 'RecursiveArrayIterator');
$ai4 = $ao4->getIterator();

$ai4->next();
$ai4->next();
$ai4->next();
var_dump($ai4->hasChildren());

$ai4->rewind();
$ao4->offsetUnset($ai4->key());
var_dump($ai4->hasChildren());

$ao4->offsetUnset($ai4->key());
var_dump($ai4->getChildren());
?>
==DONE==
<?php exit(0); ?>
--EXPECT--
NULL
NULL
int(4)
int(5)
NULL
NULL
NULL
NULL
NULL
NULL
NULL
bool(false)
bool(false)
NULL
==DONE==
