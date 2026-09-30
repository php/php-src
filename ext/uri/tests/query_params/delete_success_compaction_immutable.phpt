--TEST--
Test Uri\QueryParams::delete() - success - compacting deleted entries preserves both instances
--FILE--
<?php

$query = str_repeat("x=1&", 101) . "y=2&y=3&flag";
$params1 = Uri\QueryParams::parseRfc3986($query);
$params2 = $params1->delete("x");

var_dump($params1->toRfc3986String() === $query);
var_dump($params1->count());
var_dump($params1->getLast("x"));
var_dump($params2->toRfc3986String());
var_dump($params2->count());
var_dump($params2->getFirst("y"));
var_dump($params2->getLast("y"));
var_dump(iterator_to_array($params2) === $params2->toList());

$params2 = $params2->append("y", "4");
var_dump($params2->toRfc3986String());
var_dump($params2->count());
var_dump($params2->getLast("y"));
var_dump(iterator_to_array($params2) === $params2->toList());

?>
--EXPECT--
bool(true)
int(104)
string(1) "1"
string(12) "y=2&y=3&flag"
int(3)
string(1) "2"
string(1) "3"
bool(true)
string(16) "y=2&y=3&flag&y=4"
int(4)
string(1) "4"
bool(true)
