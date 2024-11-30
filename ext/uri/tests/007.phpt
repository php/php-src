--TEST--
Test URI creation errors
--EXTENSIONS--
uri
--FILE--
<?php

try {
    new Uri\Rfc3986Uri("https://example.com:8080@username:password/path?q=r#fragment");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
    var_dump($e->errors);
}

try {
    new Uri\WhatWgUri("https://example.com:8080@username:password/path?q=r#fragment");
} catch (Uri\InvalidUriException $e) {
    echo $e->getMessage() . "\n";
    var_dump($e->errors);
}

?>
--EXPECTF--
URI parsing failed
array(%d) {
}
URI parsing failed
array(%d) {
  [0]=>
  object(Uri\WhatWgError)#%d (%d) {
    ["context"]=>
    string(26) "password/path?q=r#fragment"
    ["type"]=>
    enum(Uri\WhatWg\WhatWgErrorType::password/path?q=r#fragment)
  }
  [1]=>
  object(Uri\WhatWgError)#%d (%d) {
    ["context"]=>
    string(36) "@username:password/path?q=r#fragment"
    ["errorCode"]=>
    int(23)
  }
}
