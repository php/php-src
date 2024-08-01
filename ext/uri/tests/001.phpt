--TEST--
Parse URL
--EXTENSIONS--
uri
--FILE--
<?php

$t1 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
   \Uri\Uri::fromRfc3986("https://example.com");
}
$t2 = hrtime(true);

$t3 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
    \Uri\Uri::fromWhatWg("https://example.com");
}
$t4 = hrtime(true);

$t5 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
    parse_url("https://example.com");
}
$t6 = hrtime(true);

echo "RFC 3986 parser: https://example.com\n";
var_dump(($t2 - $t1) / 1_000_000_000);
var_dump(Uri\Uri::fromRfc3986("https://example.com"));
echo "------------------------\n";

echo "WHATWG parser: https://example.com\n";
var_dump(($t4 - $t3) / 1_000_000_000);
var_dump(Uri\Uri::fromWhatWg("https://example.com"));
echo "------------------------\n";

echo "PHP parser: https://example.com\n";
var_dump(($t6 - $t5) / 1_000_000_000);
var_dump(parse_url("https://example.com"));
echo "------------------------\n";

?>
--EXPECTF--
RFC 3986 parser: https://example.com
float(0.006687417)
object(Uri\Uri)#%d (%d) {
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
WHATWG parser: https://example.com
float(0.013440667)
object(Uri\Uri)#%d (%d) {
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
PHP parser: https://example.com
float(0.004076416)
array(%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(11) "example.com"
}
------------------------
