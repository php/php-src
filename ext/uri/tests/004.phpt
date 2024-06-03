--TEST--
Parse invalid URLs
--EXTENSIONS--
uri
--FILE--
<?php

try {
    \Uri\Uri::fromRfc3986("");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    \Uri\Uri::fromWhatWg("");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    \Uri\Uri::fromRfc3986("https://example.com", "");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

try {
    \Uri\Uri::fromWhatWg("https://example.com", "");
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

var_dump(\Uri\Uri::fromRfc3986("192.168/contact.html"));
$errors = [];
var_dump(\Uri\Uri::fromWhatWg("192.168/contact.html", null, $errors));
var_dump($errors);

var_dump(\Uri\Uri::fromRfc3986("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's"));
$errors = [];
var_dump(\Uri\Uri::fromWhatWg("http://RuPaul's Drag Race All Stars 7 Winners Cast on This Season's", null, $errors));
var_dump($errors);

?>
--EXPECTF--
Uri\Uri::fromRfc3986(): Argument #1 ($uri) cannot be empty
Uri\Uri::fromWhatWg(): Argument #1 ($uri) cannot be empty
Uri\Uri::fromRfc3986(): Argument #2 ($baseUrl) cannot be empty
Uri\Uri::fromWhatWg(): Argument #2 ($baseUrl) cannot be empty
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
