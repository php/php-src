--TEST--
Test Uri\WhatWg\Url __construct() - error - reinitialization
--FILE--
<?php

$url = new Uri\WhatWg\Url('https://example.com');

try {
    $url->__construct('ftp://example.org');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($url);
var_dump($url->toAsciiString());

?>
--EXPECTF--
Error: Cannot modify readonly object of class Uri\WhatWg\Url
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
