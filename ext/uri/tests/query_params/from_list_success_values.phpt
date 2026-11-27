--TEST--
Test Uri\QueryParams::fromList() - success - supported value types and references
--FILE--
<?php

enum Code: int { case Ok = 200; }
enum Mode: string { case Fast = "fast"; }

$name = "code";
$value = Code::Ok;
$pair = [&$name, &$value];
$params = Uri\QueryParams::fromList([&$pair, ["mode", Mode::Fast], ["number", 42], ["ratio", 1.25]]);

$name = "changed";
$value = null;
$pair = [];
var_dump($params->toRfc3986String());

?>
--EXPECT--
string(39) "code=200&mode=fast&number=42&ratio=1.25"
