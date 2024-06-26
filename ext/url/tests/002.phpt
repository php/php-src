--TEST--
Parse URL
--EXTENSIONS--
url
--FILE--
<?php

$t1 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
    \Url\UrlParser::parseUrl("https://username:password@www.google.com:8080/pathname?query=true#hash-exists");
}
$t2 = hrtime(true);

$t3 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
    \Url\UrlParser::parseUrlArray("https://username:password@www.google.com:8080/pathname?query=true#hash-exists");
}
$t4 = hrtime(true);

$t5 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
    parse_url("https://username:password@www.google.com:8080/pathname?query=true#hash-exists");
}
$t6 = hrtime(true);

echo "Lexbor object:\n";
var_dump(($t2 - $t1) / 1_000_000_000);
var_dump(\Url\UrlParser::parseUrl("https://username:password@www.google.com:8080/pathname?query=true#hash-exists"));
echo "------------------------\n";

echo "Lexbor array:\n";
var_dump(($t4 - $t3) / 1_000_000_000);
var_dump(\Url\UrlParser::parseUrlArray("https://username:password@www.google.com:8080/pathname?query=true#hash-exists"));
echo "------------------------\n";

echo "parse_url:\n";
var_dump(($t6 - $t5) / 1_000_000_000);
var_dump(parse_url("https://username:password@www.google.com:8080/pathname?query=true#hash-exists"));
echo "------------------------\n";

?>
--EXPECTF--
Lexbor object:
float(0.018186958)
object(Url\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(14) "www.google.com"
  ["port"]=>
  int(8080)
  ["user"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["path"]=>
  string(8) "pathname"
  ["query"]=>
  string(10) "query=true"
  ["fragment"]=>
  string(11) "hash-exists"
}
------------------------
Lexbor array:
float(0.011297833)
array(%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(14) "www.google.com"
  ["port"]=>
  int(8080)
  ["user"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["path"]=>
  string(8) "pathname"
  ["query"]=>
  string(10) "query=true"
  ["fragment"]=>
  string(11) "hash-exists"
}
------------------------
parse_url:
float(0.008163625)
array(%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(14) "www.google.com"
  ["port"]=>
  int(8080)
  ["user"]=>
  string(8) "username"
  ["pass"]=>
  string(8) "password"
  ["path"]=>
  string(9) "/pathname"
  ["query"]=>
  string(10) "query=true"
  ["fragment"]=>
  string(11) "hash-exists"
}
------------------------
