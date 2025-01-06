--TEST--
Parse invalid URLs
--EXTENSIONS--
uri
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    Uri\Rfc3986\Uri::parse("");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    new Uri\WhatWg\Url("");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    Uri\WhatWg\Url::parse("");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    Uri\Rfc3986\Uri::parse("https://example.com", "");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    new Uri\Rfc3986\Uri("https://example.com", "");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    new Uri\WhatWg\Url("https://example.com", "");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    Uri\WhatWg\Url::parse("https://example.com", "");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

var_dump(Uri\Rfc3986\Uri::parse("192.168/contact.html"));
var_dump(Uri\WhatWg\Url::parse("192.168/contact.html", null));

var_dump(Uri\Rfc3986\Uri::parse("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's"));
var_dump(Uri\WhatWg\Url::parse("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's", null));

?>
--EXPECTF--
Uri\Rfc3986\Uri::__construct(): Argument #1 ($uri) cannot be empty
Uri\Rfc3986\Uri::parse(): Argument #1 ($uri) cannot be empty
Uri\WhatWg\Url::__construct(): Argument #1 ($uri) cannot be empty
Uri\WhatWg\Url::parse(): Argument #1 ($uri) cannot be empty
Uri\Rfc3986\Uri::parse(): Argument #2 ($baseUrl) cannot be empty
Uri\Rfc3986\Uri::__construct(): Argument #2 ($baseUrl) cannot be empty
Uri\WhatWg\Url::__construct(): Argument #2 ($baseUrl) cannot be empty
Uri\WhatWg\Url::parse(): Argument #2 ($baseUrl) cannot be empty
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  NULL
  ["user"]=>
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
