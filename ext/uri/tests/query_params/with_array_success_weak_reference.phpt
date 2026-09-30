--TEST--
Test Uri\QueryParams::withArray() - success - weak references prevent object reuse
--FILE--
<?php

$params = Uri\QueryParams::fromArray(["old" => "1"]);
$reference = WeakReference::create($params);
$params = $params->withArray(["new" => "2"]);

var_dump($reference->get());
var_dump($params->getFirst("old"));
var_dump($params->getFirst("new"));

?>
--EXPECT--
NULL
NULL
string(1) "2"
