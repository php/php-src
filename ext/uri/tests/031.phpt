--TEST--
Test serialization and unserialization
--EXTENSIONS--
uri
--FILE--
<?php

$uri1 = new Uri\Rfc3986\Uri("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
$serializedUri1 = serialize($uri1);
$uri2 = unserialize($serializedUri1);

var_dump($serializedUri1);
var_dump($uri2);

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":1:{i:0;a:0:{}}'); // less than 2 items
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":3:{i:0;a:0:{}i:1;a:0:{}i:2;a:0:{}}'); // more than 2 items
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":2:{i:0;N;i:1;a:0:{}}'); // first item is not an array
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":2:{i:0;a:0:{}i:1;a:0:{}}'); // first array is empty
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":2:{i:0;a:1:{s:3:"uri";i:1;}i:1;a:0:{}}'); // "uri" key in first array is not a string
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":2:{i:0;a:1:{s:3:"uri";s:2:"%1";}i:1;a:0:{}}'); // "uri" key in first array contains invalid URI
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":2:{i:0;a:1:{s:3:"uri";s:4:"/uri";}i:1;s:0:"";}'); // second item in not an array
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:15:"Uri\Rfc3986\Uri":2:{i:0;a:1:{s:3:"uri";s:4:"/uri";}i:1;a:1:{s:5:"prop1";i:123;}}'); // second array contains a property
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

$url1 = new Uri\WhatWg\Url("https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists");
$serializedUrl1 = serialize($url1);
$url2 = unserialize($serializedUrl1);

var_dump($serializedUrl1);
var_dump($url2);

try {
    unserialize('O:14:"Uri\WhatWg\Url":1:{i:0;a:0:{}}'); // less than 2 items
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":3:{i:0;a:0:{}i:1;a:0:{}i:2;a:0:{}}'); // more than 2 items
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;N;i:1;a:0:{}}'); // first item is not an array
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:0:{}i:1;a:0:{}}'); // first array is empty
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:2:{s:3:"uri";s:19:"https://example.com";s:1:"a";i:1;}i:1;a:0:{}}'); // "uri" key in first array contains more than 1 item
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:1:{s:3:"uri";i:1;}i:1;a:0:{}}'); // "uri" key in first array is not a string
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:1:{s:3:"uri";s:11:"invalid-url";}i:1;a:0:{}}'); // "uri" key in first array contains invalid URL
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:1:{s:3:"uri";s:19:"https://example.com";}i:1;s:0:"";}'); // second item in not an array
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:1:{s:3:"uri";s:19:"https://example.com";}i:1;a:1:{s:5:"prop1";i:123;}}'); // second array contains property
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
string(163) "O:15:"Uri\Rfc3986\Uri":2:{i:0;a:1:{s:3:"uri";s:98:"https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists";}i:1;a:0:{}}"
object(Uri\Rfc3986\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["host"]=>
  string(14) "www.google.com"
  ["port"]=>
  int(8080)
  ["path"]=>
  string(30) "/pathname1/pathname2/pathname3"
  ["query"]=>
  string(10) "query=true"
  ["fragment"]=>
  string(11) "hash-exists"
}
Invalid serialization data for Uri\Rfc3986\Uri object
Invalid serialization data for Uri\Rfc3986\Uri object
Invalid serialization data for Uri\Rfc3986\Uri object
Invalid serialization data for Uri\Rfc3986\Uri object
Invalid serialization data for Uri\Rfc3986\Uri object
Invalid serialization data for Uri\Rfc3986\Uri object
Invalid serialization data for Uri\Rfc3986\Uri object
Invalid serialization data for Uri\Rfc3986\Uri object
string(162) "O:14:"Uri\WhatWg\Url":2:{i:0;a:1:{s:3:"uri";s:98:"https://username:password@www.google.com:8080/pathname1/pathname2/pathname3?query=true#hash-exists";}i:1;a:0:{}}"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  string(8) "username"
  ["password"]=>
  string(8) "password"
  ["host"]=>
  string(14) "www.google.com"
  ["port"]=>
  int(8080)
  ["path"]=>
  string(30) "/pathname1/pathname2/pathname3"
  ["query"]=>
  string(10) "query=true"
  ["fragment"]=>
  string(11) "hash-exists"
}
Invalid serialization data for Uri\WhatWg\Url object
Invalid serialization data for Uri\WhatWg\Url object
Invalid serialization data for Uri\WhatWg\Url object
Invalid serialization data for Uri\WhatWg\Url object
Invalid serialization data for Uri\WhatWg\Url object
Invalid serialization data for Uri\WhatWg\Url object
Invalid serialization data for Uri\WhatWg\Url object
Invalid serialization data for Uri\WhatWg\Url object
Invalid serialization data for Uri\WhatWg\Url object
