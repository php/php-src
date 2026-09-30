--TEST--
Test Uri\QueryParams::sortByBytes() - success - a weakly referenced temporary instance is cloned
--FILE--
<?php

function createParams(&$reference): Uri\QueryParams {
    $params1 = (new Uri\QueryParams())->append("z", "1")->append("a", "2");
    $reference = WeakReference::create($params1);
    return $params1;
}

$params2 = createParams($reference)->sortByBytes();

var_dump($reference->get());
var_dump($params2->toList());
var_dump($params2->toRfc3986String());

?>
--EXPECT--
NULL
array(2) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "a"
    [1]=>
    string(1) "2"
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "z"
    [1]=>
    string(1) "1"
  }
}
string(7) "a=2&z=1"
