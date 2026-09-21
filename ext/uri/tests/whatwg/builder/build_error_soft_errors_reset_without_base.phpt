--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - clears soft errors when an exception is thrown
--FILE--
<?php

$referenceErrors = [];

try {
    new Uri\WhatWg\Url("ht\ttps://");
} catch (Throwable $e) {
    $referenceErrors = $e->errors;
}

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("ht\ttps");
$builder->setHost(null);
$softErrors = ["previous error"];

try {
    $builder->build(softErrors: $softErrors);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors);
    var_dump(
        array_map(static fn($error) => $error->type, $e->errors)
        === array_map(static fn($error) => $error->type, $referenceErrors)
    );
}

var_dump($softErrors);

?>
--EXPECTF--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (HostMissing)
array(2) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(0) ""
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::HostMissing)
    ["failure"]=>
    bool(true)
  }
  [1]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(4) "	tps"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
array(0) {
}
