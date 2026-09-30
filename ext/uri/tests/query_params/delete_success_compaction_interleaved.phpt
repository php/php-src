--TEST--
Test Uri\QueryParams::delete() - success - compacting interleaved entries and compacting again
--FILE--
<?php

$query = "x=0&y=first&" . str_repeat("x=0&y=middle&", 100) . "x=0&flag&y=last&x=0";
$params1 = Uri\QueryParams::parseRfc3986($query);
$params2 = $params1->delete("x");

var_dump($params1->toRfc3986String() === $query);
var_dump($params2->toRfc3986String() === "y=first&" . str_repeat("y=middle&", 100) . "flag&y=last");
var_dump($params2->count());
var_dump($params2->has("x"));
var_dump($params2->getFirst("y"));
var_dump($params2->getLast("y"));
var_dump($params2->getAll("y") === array_merge(["first"], array_fill(0, 100, "middle"), ["last"]));
var_dump(iterator_to_array($params2) === $params2->toList());

$params2 = $params2->append("y", "appended");
var_dump($params2->getLast("y"));
$params2 = $params2->deleteValue("y", "middle");
var_dump($params2->toRfc3986String());

// Removing the remaining duplicate names triggers compaction a second time.
$params2 = $params2->set("y", "only");
var_dump($params2->toRfc3986String());
var_dump($params2->count());
var_dump($params2->getFirst("y"));
var_dump($params2->getLast("y"));
var_dump($params2->has("flag"));
var_dump($params2->getFirst("flag"));
var_dump(iterator_to_array($params2) === $params2->toList());

?>
--EXPECT--
bool(true)
bool(true)
int(103)
bool(false)
string(5) "first"
string(4) "last"
bool(true)
bool(true)
string(8) "appended"
string(30) "y=first&flag&y=last&y=appended"
string(11) "y=only&flag"
int(2)
string(4) "only"
string(4) "only"
bool(true)
NULL
bool(true)
