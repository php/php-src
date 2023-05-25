--TEST--
Parse URL
--EXTENSIONS--
url
--FILE--
<?php

$t1 = microtime(true);
for ($i = 0; $i < 1000; $i++) {
    Url\URlParser::parseUrl("https://example.com");
}
$t2 = microtime(true);

$t3 = microtime(true);
for ($i = 0; $i < 1000; $i++) {
    Url\URlParser::parseUrlArray("https://example.com");
}
$t4 = microtime(true);

$t5 = microtime(true);
for ($i = 0; $i < 1000; $i++) {
    parse_url("https://example.com");
}
$t6 = microtime(true);

var_dump("ADA object: https://example.com");
var_dump($t2-$t1);
var_dump(Url\URlParser::parseUrl("https://example.com"));
echo "------------------------\n";

var_dump("ADA array: https://example.com");
var_dump($t4-$t3);
var_dump(Url\URlParser::parseUrlArray("https://example.com"));
echo "------------------------\n";

var_dump("PHP: https://example.com");
var_dump($t6-$t5);
var_dump(parse_url("https://example.com"));
echo "------------------------\n";

?>
--EXPECTF--
