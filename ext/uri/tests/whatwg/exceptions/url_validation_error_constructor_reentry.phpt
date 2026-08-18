--TEST--
Test Uri\WhatWg\UrlValidationError constructor reentry stops after readonly failures
--FILE--
<?php

$r = new Uri\WhatWg\UrlValidationError('foo', Uri\WhatWg\UrlValidationErrorType::DomainInvalidCodePoint, true);

try {
    $r->__construct('bar', Uri\WhatWg\UrlValidationErrorType::HostMissing, false);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($r);

foreach ([
    'O:29:"Uri\\WhatWg\\UrlValidationError":1:{s:4:"type";E:45:"Uri\\WhatWg\\UrlValidationErrorType:HostMissing";}',
    'O:29:"Uri\\WhatWg\\UrlValidationError":1:{s:7:"failure";b:1;}',
] as $serialized) {

    $r = unserialize($serialized);

    try {
        $r->__construct('foo', Uri\WhatWg\UrlValidationErrorType::HostMissing, false);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
Error: Cannot modify readonly property Uri\WhatWg\UrlValidationError::$context
object(Uri\WhatWg\UrlValidationError)#%d (%d) {
  ["context"]=>
  string(3) "foo"
  ["type"]=>
  enum(Uri\WhatWg\UrlValidationErrorType::DomainInvalidCodePoint)
  ["failure"]=>
  bool(true)
}
Error: Cannot modify readonly property Uri\WhatWg\UrlValidationError::$type
Error: Cannot modify readonly property Uri\WhatWg\UrlValidationError::$failure
