--TEST--
Test resolve() method - error cases
--EXTENSIONS--
uri
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri("https://example.com");

try {
    $uri->resolve("á");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
}

$url = new Uri\WhatWg\Url("https://example.com");

try {
    $url->resolve("https://1.2.3.4.5");
} catch (Uri\WhatWg\InvalidUrlException $e) {
    echo $e->getMessage() . "\n";
}

$softErrors = [];

var_dump($url->resolve(" /foo", $softErrors)->toAsciiString());
var_dump($softErrors);

?>
--EXPECTF--
The specified URI is malformed
The specified URI is malformed (Ipv4TooManyParts)
string(23) "https://example.com/foo"
array(%d) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(5) " /foo"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
