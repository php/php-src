--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - port with a host that normalizes to empty
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("foo");
$builder->setHost("\t\n");
$builder->setPort(123);
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
Uri\WhatWg\InvalidUrlException: The specified URL cannot have port
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(2) "	
"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
array(1) {
  [0]=>
  string(9) "unchanged"
}
