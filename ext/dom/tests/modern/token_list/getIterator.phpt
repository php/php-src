--TEST--
TokenList: getIterator
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="A B C"/>');
$element = $dom->documentElement;
$list = $element->classList;

$it = $list->getIterator();
var_dump($it);

var_dump($it->key(), $it->current());
$it->next();
var_dump($it->key(), $it->current());
$it->next();
var_dump($it->key(), $it->current());
$it->next();
var_dump($it->key(), $it->current());

$it->rewind();
var_dump($it->key(), $it->current());

?>
--EXPECT--
object(InternalIterator)#5 (0) {
}
int(0)
string(1) "A"
int(1)
string(1) "B"
int(2)
string(1) "C"
int(3)
NULL
int(0)
string(1) "A"
