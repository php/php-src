--TEST--
Test IDNA support
--EXTENSIONS--
uri
--FILE--
<?php

$uri = Uri\Rfc3986\Uri::parse("🐘");
var_dump($uri);

$uri = Uri\WhatWg\Url::parse("🐘", null);
var_dump($uri);
var_dump($uri[0]->errorCode === \Uri\WhatWgError::ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL);

?>
--EXPECTF--
NULL
array(1) {
  [0]=>
  object(Uri\WhatWgError)#%d (%d) {
    ["context"]=>
    string(4) "🐘"
    ["errorCode"]=>
    int(21)
  }
}
bool(true)
