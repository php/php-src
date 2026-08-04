--TEST--
Test IDNA support
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("🐘");
var_dump($uri);

$errors = [];
$url = Uri\WhatWg\Url::parse("🐘", null, $errors);
var_dump($url);
var_dump($errors);

$uri = Uri\Rfc3986\Uri::parse("https://🐘.com/🐘?🐘=🐘");
var_dump($uri);

$url = Uri\WhatWg\Url::parse("https://🐘.com/🐘?🐘=🐘", null);
var_dump($url);
var_dump($url->getAsciiHost());
var_dump($url->getUnicodeHost());
var_dump($url->toAsciiString());
var_dump($url->toUnicodeString());

?>
--EXPECTF--
NULL
NULL
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(4) "🐘"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::MissingSchemeNonRelativeUrl)
    ["failure"]=>
    bool(true)
  }
}
NULL
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(12) "xn--go8h.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(13) "/%F0%9F%90%98"
  ["query"]=>
  string(25) "%F0%9F%90%98=%F0%9F%90%98"
  ["fragment"]=>
  NULL
}
string(12) "xn--go8h.com"
string(8) "🐘.com"
string(59) "https://xn--go8h.com/%F0%9F%90%98?%F0%9F%90%98=%F0%9F%90%98"
string(55) "https://🐘.com/%F0%9F%90%98?%F0%9F%90%98=%F0%9F%90%98"
