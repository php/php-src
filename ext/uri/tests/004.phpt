--TEST--
Parse invalid URLs
--EXTENSIONS--
uri
--FILE--
<?php

var_dump(new Uri\Rfc3986\Uri(""));
var_dump(Uri\Rfc3986\Uri::parse(""));

try {
    new Uri\WhatWg\Url("");
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

var_dump(Uri\WhatWg\Url::parse(""));

var_dump(Uri\Rfc3986\Uri::parse("192.168/contact.html"));
var_dump(Uri\WhatWg\Url::parse("192.168/contact.html", null));

var_dump(Uri\Rfc3986\Uri::parse("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's"));
var_dump(Uri\WhatWg\Url::parse("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's", null));

?>
--EXPECTF--
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
The specified URI is malformed (MissingSchemeNonRelativeUrl)
NULL
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  NULL
  ["port"]=>
  NULL
  ["path"]=>
  string(20) "192.168/contact.html"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
NULL
NULL
NULL
