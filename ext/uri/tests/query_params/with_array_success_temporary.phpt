--TEST--
Test Uri\QueryParams::withArray() - success - reuse a temporary instance containing tombstones
--FILE--
<?php

function createParams(&$id): Uri\QueryParams {
    $params1 = (new Uri\QueryParams())->append("old", "1")->append("keep", "2")->delete("old");
    $id = spl_object_id($params1);
    return $params1;
}

$params2 = createParams($id)->withArray(["new" => ["a", "b"]]);

var_dump(spl_object_id($params2) === $id);
var_dump($params2->toList());
var_dump($params2->toRfc3986String());
var_dump($params2->has("old"));
var_dump($params2->has("keep"));
var_dump($params2->getAll("new[]"));
var_dump($params2->count());

?>
--EXPECT--
bool(true)
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(5) "new[]"
    [1]=>
    string(1) "a"
  }
  [1]=>
  array(2) {
    [0]=>
    string(5) "new[]"
    [1]=>
    string(1) "b"
  }
}
string(23) "new%5B%5D=a&new%5B%5D=b"
bool(false)
bool(false)
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
int(2)
