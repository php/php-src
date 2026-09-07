--TEST--
GH-16371: Assertion failure in zend_weakmap_iterator_get_current_key() for invalid iterator
--FILE--
<?php

$map = new WeakMap();
$it = $map->getIterator();

print "# Empty WeakMap\n";

var_dump($it->key());
var_dump($it->current());
var_dump($it->valid());

$map = new WeakMap();
$obj = new stdClass;
$map[$obj] = 0;

print "# Valid iterator\n";

$it = $map->getIterator();
var_dump($it->key());
var_dump($it->current());
var_dump($it->valid());

print "# End of iterator\n";

$it->next();
var_dump($it->key());
var_dump($it->current());
var_dump($it->valid());

?>
--EXPECTF--
# Empty WeakMap
NULL
NULL
bool(false)
# Valid iterator
object(stdClass)#%d (0) {
}
int(0)
bool(true)
# End of iterator
NULL
NULL
bool(false)
