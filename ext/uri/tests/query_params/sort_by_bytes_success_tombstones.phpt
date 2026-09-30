--TEST--
Test Uri\QueryParams::sortByBytes() - success - deleted entries and lookup indices
--FILE--
<?php

$params1 = (new Uri\QueryParams())
    ->append("gone", "0")->append("z", "first")->append("a", "only")
    ->append("z", "removed")->append("b", "only")->append("z", "last")
    ->delete("gone")->deleteValue("z", "removed");
$iterator = $params1->getIterator();
$params2 = $params1->sortByBytes();

var_dump($params1->toRfc3986String());
var_dump($params2->toRfc3986String());
var_dump(iterator_to_array($iterator) === $params1->toList());
var_dump(iterator_to_array($params2) === $params2->toList());
var_dump($params2->count());
var_dump($params2->has("gone"));
var_dump($params2->getFirst("z"));
var_dump($params2->getLast("z"));
var_dump($params2->getAll("z"));

$params2 = $params2->append("z", "appended");
var_dump($params2->getAll("z"));
$params2 = $params2->deleteValue("z", "first");
var_dump($params2->getFirst("z"));
$params2 = $params2->set("z", "replaced");
var_dump($params2->toRfc3986String());
$params2 = $params2->delete("z")->delete("a")->sortByBytes();
var_dump($params2->toList());
$params2 = $params2->append("b", "second");
var_dump($params2->getAll("b"));
var_dump(iterator_to_array($params2) === $params2->toList());

?>
--EXPECT--
string(28) "z=first&a=only&b=only&z=last"
string(28) "a=only&b=only&z=first&z=last"
bool(true)
bool(true)
int(4)
bool(false)
string(5) "first"
string(4) "last"
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(4) "last"
}
array(3) {
  [0]=>
  string(5) "first"
  [1]=>
  string(4) "last"
  [2]=>
  string(8) "appended"
}
string(4) "last"
string(24) "a=only&b=only&z=replaced"
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "b"
    [1]=>
    string(4) "only"
  }
}
array(2) {
  [0]=>
  string(4) "only"
  [1]=>
  string(6) "second"
}
bool(true)
