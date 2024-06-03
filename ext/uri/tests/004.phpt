--TEST--
Parse invalid URLs
--EXTENSIONS--
uri
--FILE--
<?php

try {
    Uri\Rfc3986Uri::create("");
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
    Uri\WhatWgUri::create("https://example.com", "");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

var_dump(Uri\Rfc3986Uri::create("192.168/contact.html"));
$errors = [];
var_dump(Uri\WhatWgUri::create("192.168/contact.html", null, $errors));
var_dump($errors);

var_dump(Uri\Rfc3986Uri::create("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's"));
$errors = [];
var_dump(Uri\WhatWgUri::create("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's", null, $errors));
var_dump($errors);

?>
--EXPECTF--
Uri\Rfc3986Uri::create(): Argument #1 ($uri) cannot be empty
Uri\WhatWgUri::create(): Argument #1 ($uri) cannot be empty
Uri\Rfc3986Uri::create(): Argument #2 ($baseUrl) cannot be empty
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
NULL
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
