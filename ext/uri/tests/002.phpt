--TEST--
Parse URL
--EXTENSIONS--
uri
--FILE--
<?php

$t1 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
    Uri\Rfc3986\Uri::parse("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
}
$t2 = hrtime(true);

$t3 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
    Uri\WhatWg\Url::parse("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
}
$t4 = hrtime(true);

$t5 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
    parse_url("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
}
$t6 = hrtime(true);

echo "RFC 3986 parser:\n";
var_dump(($t2 - $t1) / 1_000_000_000);
var_dump(Uri\Rfc3986\Uri::parse("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists"));
echo "------------------------\n";

echo "WHATWG parser:\n";
var_dump(($t4 - $t3) / 1_000_000_000);
var_dump(Uri\WhatWg\Url::parse("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists"));
echo "------------------------\n";

echo "parse_url:\n";
var_dump(($t6 - $t5) / 1_000_000_000);
var_dump(parse_url("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists"));
echo "------------------------\n";

?>
--EXPECTF--
RFC 3986 parser:
float(0.004389959)
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["userinfo"]=>
  string(17) "username:password"
  ["username"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["host"]=>
  string(14) "www.google.com"
  ["port"]=>
  int(8080)
  ["path"]=>
  string(30) "/pathname1/pathname2/pathname3"
  ["query"]=>
  string(10) "query=true"
  ["fragment"]=>
  string(11) "hash-exists"
}
------------------------
WHATWG parser:
float(0.007400166)
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["host"]=>
  string(14) "www.google.com"
  ["port"]=>
  int(8080)
  ["path"]=>
  string(30) "/pathname1/pathname2/pathname3"
  ["query"]=>
  string(10) "query=true"
  ["fragment"]=>
  string(11) "hash-exists"
}
------------------------
parse_url:
float(0.007400166)
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
  string(30) "/pathname1/pathname2/pathname3"
  ["query"]=>
  string(10) "query=true"
  ["fragment"]=>
  string(11) "hash-exists"
}
------------------------
