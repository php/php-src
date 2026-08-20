--TEST--
Test Uri\WhatWg\Url __unserialize() - error - reinitialization
--FILE--
<?php

$url = new Uri\WhatWg\Url('https://example.com');

try {
    $url->__unserialize([['uri' => 'ftp://example.org'], []]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($url);
var_dump($url->toUnicodeString());
var_dump($url->toAsciiString());

?>
--EXPECTF--
Exception: Invalid serialization data for Uri\WhatWg\Url object
object(Uri\WhatWg\Url)#%d (8) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(20) "https://example.com/"
string(20) "https://example.com/"
