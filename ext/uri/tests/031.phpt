--TEST--
Test serialization and unserialization
--EXTENSIONS--
uri
--FILE--
<?php

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
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:1:{s:3:"uri";i:1;}i:1;a:0:{}}'); // "uri" key in first array is not a string
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:14:"Uri\WhatWg\Url":2:{i:0;a:1:{s:3:"uri";s:11:"invalid-url";}i:1;a:0:{}}'); // "uri" key in first array contains invalid URL
} catch (Uri\WhatWg\InvalidUrlException $e) {
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
URL parsing failed
Invalid serialization data for Uri\WhatWg\Url object
Invalid serialization data for Uri\WhatWg\Url object
