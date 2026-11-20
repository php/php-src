--TEST--
Test Uri\QueryParams::toArray() - success - NUL bytes and non-UTF-8 bytes are preserved
--FILE--
<?php

$params = new Uri\QueryParams();
$params = $params->append("\0root[\0key]", "\0\xff")->append("bad[\0", null);

var_dump($params->toArray() === ["\0root" => ["\0key" => "\0\xff"], "bad[\0" => null]);

?>
--EXPECT--
bool(true)
