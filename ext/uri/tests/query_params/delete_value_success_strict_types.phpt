--TEST--
Test Uri\QueryParams::deleteValue() - success - scalar conversion with strict types
--FILE--
<?php

declare(strict_types=1);

$params1 = Uri\QueryParams::parseRfc3986("int=123&float=1.25&keep=yes");
$params2 = $params1->deleteValue("int", 123)->deleteValue("float", 1.25);

var_dump($params1->toRfc3986String());
var_dump($params2->toRfc3986String());

?>
--EXPECT--
string(27) "int=123&float=1.25&keep=yes"
string(8) "keep=yes"
