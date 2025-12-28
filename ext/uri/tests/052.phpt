--TEST--
Test UrlValidationError constructor error handling
--EXTENSIONS--
uri
--FILE--
<?php

$r = new Uri\WhatWg\UrlValidationError('foo', Uri\WhatWg\UrlValidationErrorType::DomainInvalidCodePoint, true);

try {
    $r->__construct('bar', Uri\WhatWg\UrlValidationErrorType::HostMissing, false);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump($r);

?>
--EXPECTF--
Cannot modify readonly property Uri\WhatWg\UrlValidationError::$context
object(Uri\WhatWg\UrlValidationError)#%d (%d) {
  ["context"]=>
  string(3) "foo"
  ["type"]=>
  enum(Uri\WhatWg\UrlValidationErrorType::DomainInvalidCodePoint)
  ["failure"]=>
  bool(true)
}
