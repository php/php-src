--TEST--
Parse URL
--EXTENSIONS--
uri
--FILE--
<?php

$t1 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
   Uri\Rfc3986\Uri::parse("https://example.com/");
}
$t2 = hrtime(true);

$t3 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
    Uri\WhatWg\Url::parse("https://example.com/");
}
$t4 = hrtime(true);

$t5 = hrtime(true);
for ($i = 0; $i < 1000; $i++) {
    parse_url("https://example.com/");
}
$t6 = hrtime(true);

echo "RFC 3986 parser: https://example.com/\n";
var_dump(($t2 - $t1) / 1_000_000_000);
var_dump(Uri\Rfc3986\Uri::parse("https://example.com/"));
echo "------------------------\n";

echo "WHATWG parser: https://example.com/\n";
var_dump(($t4 - $t3) / 1_000_000_000);
var_dump(Uri\WhatWg\Url::parse("https://example.com"));
echo "------------------------\n";

echo "PHP parser: https://example.com/\n";
var_dump(($t6 - $t5) / 1_000_000_000);
var_dump(parse_url("https://example.com"));
echo "------------------------\n";

?>
--EXPECTF--
RFC 3986 parser: https://example.com/
float(0.0032325)
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["userinfo"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
------------------------
WHATWG parser: https://example.com/
float(0.004510875)
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
------------------------
PHP parser: https://example.com/
float(0.004519541)
array(%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(11) "example.com"
}
------------------------
