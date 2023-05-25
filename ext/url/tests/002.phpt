--TEST--
Parse URL
--EXTENSIONS--
url
--FILE--
<?php

$t1 = microtime(true);
for ($i = 0; $i < 1000; $i++) {
    \Url\UrlParser::parseUrl("https://username:password@www.google.com:8080/pathname?query=true#hash-exists");
}
$t2 = microtime(true);

$t3 = microtime(true);
for ($i = 0; $i < 1000; $i++) {
    \Url\UrlParser::parseUrlArray("https://username:password@www.google.com:8080/pathname?query=true#hash-exists");
}
$t4 = microtime(true);

$t5 = microtime(true);
for ($i = 0; $i < 1000; $i++) {
    parse_url("https://username:password@www.google.com:8080/pathname?query=true#hash-exists");
}
$t6 = microtime(true);

echo "ADA object:\n";
var_dump($t2-$t1);
var_dump(\Url\UrlParser::parseUrl("https://username:password@www.google.com:8080/pathname?query=true#hash-exists"));
echo "------------------------\n";

echo "ADA array:\n";
var_dump($t4-$t3);
var_dump(\Url\UrlParser::parseUrlArray("https://username:password@www.google.com:8080/pathname?query=true#hash-exists"));
echo "------------------------\n";

echo "parse_url:\n";
var_dump($t6-$t5);
var_dump(parse_url("https://username:password@www.google.com:8080/pathname?query=true#hash-exists"));
echo "------------------------\n";

?>
--EXPECTF--
