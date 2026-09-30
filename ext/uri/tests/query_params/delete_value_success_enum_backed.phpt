--TEST--
Test Uri\QueryParams::deleteValue() - success - string and integer backed enums
--FILE--
<?php

enum StringEnum: string {
    case Bar = "bar";
}

enum IntEnum: int {
    case Baz = 123;
}

$params1 = Uri\QueryParams::parseRfc3986("foo=bar&baz=123&keep=yes");
$params2 = $params1->deleteValue("foo", StringEnum::Bar)->deleteValue("baz", IntEnum::Baz);

var_dump($params1->toRfc3986String());
var_dump($params2->toRfc3986String());

?>
--EXPECT--
string(24) "foo=bar&baz=123&keep=yes"
string(8) "keep=yes"
