--TEST--
Test URI creation errors
--EXTENSIONS--
uri
--FILE--
<?php

try {
    new Uri\Rfc3986\Uri("https://example.com:8080@username:password/path?q=r#fragment");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
    var_dump($e->errors);
}

try {
    new Uri\WhatWg\Url("https://example.com:8080@username:password/path?q=r#fragment");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
    var_dump($e->errors);
}

$failures = [];
$url = new Uri\WhatWg\Url(" https://example.org ", null, $failures);
var_dump($url->toString());
var_dump($failures);

?>
--EXPECTF--
URI parsing failed
array(%d) {
}
URI parsing failed
array(%d) {
  [0]=>
  object(Uri\WhatWg\WhatWgError)#%d (%d) {
    ["context"]=>
    string(26) "password/path?q=r#fragment"
    ["type"]=>
    enum(Uri\WhatWg\WhatWgErrorType::PortInvalid)
    ["failure"]=>
    bool(true)
  }
  [1]=>
  object(Uri\WhatWg\WhatWgError)#%d (%d) {
    ["context"]=>
    string(36) "@username:password/path?q=r#fragment"
    ["type"]=>
    enum(Uri\WhatWg\WhatWgErrorType::InvalidCredentials)
    ["failure"]=>
    bool(false)
  }
}
string(20) "https://example.org/"
array(2) {
  [0]=>
  object(Uri\WhatWg\WhatWgError)#%d (%d) {
    ["context"]=>
    string(1) " "
    ["type"]=>
    enum(Uri\WhatWg\WhatWgErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
  [1]=>
  object(Uri\WhatWg\WhatWgError)#%d (%d) {
    ["context"]=>
    string(21) " https://example.org "
    ["type"]=>
    enum(Uri\WhatWg\WhatWgErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
