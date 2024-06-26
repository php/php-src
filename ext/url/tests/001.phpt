--TEST--
Parse URL
--EXTENSIONS--
url
--FILE--
<?php

$t1 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
    Url\URlParser::parseUrl("https://example.com");
}
$t2 = hrtime(true);

$t3 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
    Url\URlParser::parseUrlArray("https://example.com");
}
$t4 = hrtime(true);

$t5 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
    parse_url("https://example.com");
}
$t6 = hrtime(true);

var_dump("Lexbor object: https://example.com");
var_dump(($t2 - $t1) / 1_000_000_000);
var_dump(Url\URlParser::parseUrl("https://example.com"));
echo "------------------------\n";

var_dump("Lexbor array: https://example.com");
var_dump(($t4 - $t3) / 1_000_000_000);
var_dump(Url\URlParser::parseUrlArray("https://example.com"));
echo "------------------------\n";

var_dump("PHP: https://example.com");
var_dump(($t6 - $t5) / 1_000_000_000);
var_dump(parse_url("https://example.com"));
echo "------------------------\n";

?>
--EXPECTF--
string(34) "Lexbor object: https://example.com"
float(0.013440667)
object(Url\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
------------------------
string(33) "Lexbor array: https://example.com"
float(0.006687417)
array(%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(11) "example.com"
}
------------------------
string(24) "PHP: https://example.com"
float(0.004076416)
array(%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(11) "example.com"
}
------------------------
