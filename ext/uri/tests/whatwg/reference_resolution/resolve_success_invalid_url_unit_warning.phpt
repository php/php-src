--TEST--
Test Uri\WhatWg\Url reference resolution - resolve() - invalid URL unit warning
--FILE--
<?php

$url = new Uri\WhatWg\Url("https://example.com");
$softErrors = [];

var_dump($url->resolve(" /foo", $softErrors)->toAsciiString());
var_dump($softErrors);

?>
--EXPECTF--
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
