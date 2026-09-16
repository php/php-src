--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - leaves soft errors unchanged
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("ht\ttps");
$builder->setHost(null);
$softErrors = ["unchanged"];

try {
    $builder->build(softErrors: $softErrors);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors);
}

var_dump($softErrors);

?>
--EXPECTF--
Uri\WhatWg\InvalidUrlException: The specified host is malformed (HostMissing)
array(2) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(4) "	tps"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
  [1]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(0) ""
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::HostMissing)
    ["failure"]=>
    bool(true)
  }
}
array(1) {
  [0]=>
  string(9) "unchanged"
}
