--TEST--
Test InvalidUrlException constructor error handling
--EXTENSIONS--
uri
--FILE--
<?php

$r = new Uri\WhatWg\InvalidUrlException(
    "",
    [new Uri\WhatWg\UrlValidationError("abc", Uri\WhatWg\UrlValidationErrorType::DomainInvalidCodePoint, true)],
    1,
    new Exception()
);

try {
    $r->__construct("foo");
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $r->__construct("bar", []);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $r->__construct("baz", [], 0);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $r->__construct("qax", [], 0, null);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

var_dump($r->getMessage());
var_dump($r->errors);
var_dump($r->getCode());
var_dump($r->getPrevious()::class);

?>
--EXPECTF--
Cannot modify readonly property Uri\WhatWg\InvalidUrlException::$errors
Cannot modify readonly property Uri\WhatWg\InvalidUrlException::$errors
Cannot modify readonly property Uri\WhatWg\InvalidUrlException::$errors
Cannot modify readonly property Uri\WhatWg\InvalidUrlException::$errors
string(3) "qax"
array(%d) {
  [%d]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(3) "abc"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::DomainInvalidCodePoint)
    ["failure"]=>
    bool(true)
  }
}
int(1)
string(9) "Exception"
