--TEST--
Parse invalid URLs
--EXTENSIONS--
uri
--FILE--
<?php

try {
    new Uri\Rfc3986Uri("");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    Uri\Rfc3986Uri::create("");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    new Uri\WhatWgUri("");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    Uri\WhatWgUri::create("");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    Uri\Rfc3986Uri::create("https://example.com", "");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    new Uri\Rfc3986Uri("https://example.com", "");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    new Uri\WhatWgUri("https://example.com", "");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    Uri\WhatWgUri::create("https://example.com", "");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

var_dump(Uri\Rfc3986Uri::create("192.168/contact.html"));
var_dump(Uri\WhatWgUri::create("192.168/contact.html", null));

var_dump(Uri\Rfc3986Uri::create("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's"));
var_dump(Uri\WhatWgUri::create("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's", null));

?>
--EXPECTF--
Uri\Rfc3986Uri::__construct(): Argument #1 ($uri) cannot be empty
Uri\Rfc3986Uri::create(): Argument #1 ($uri) cannot be empty
Uri\WhatWgUri::__construct(): Argument #1 ($uri) cannot be empty
Uri\WhatWgUri::create(): Argument #1 ($uri) cannot be empty
Uri\Rfc3986Uri::create(): Argument #2 ($baseUrl) cannot be empty
Uri\Rfc3986Uri::__construct(): Argument #2 ($baseUrl) cannot be empty
Uri\WhatWgUri::__construct(): Argument #2 ($baseUrl) cannot be empty
Uri\WhatWgUri::create(): Argument #2 ($baseUrl) cannot be empty
object(Uri\Rfc3986Uri)#%d (%d) {
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
array(%d) {
  [0]=>
  object(Uri\WhatWgError)#%d (%d) {
    ["position"]=>
    string(20) "192.168/contact.html"
    ["errorCode"]=>
    int(21)
  }
}
NULL
array(%d) {
  [0]=>
  object(Uri\WhatWgError)#%d (%d) {
    ["position"]=>
    string(52) " drag race all stars 7 winners cast on this season's"
    ["errorCode"]=>
    int(2)
  }
}
