--TEST--
Test that overwriting Uri\WhatWg\Url is not possible
--FILE--
<?php

$url = new Uri\WhatWg\Url('https://example.com');
try {
    $url->__construct('ftp://example.org');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($url);

$url = new Uri\WhatWg\Url('https://example.com');
try {
    $url->__unserialize([['uri' => 'ftp://example.org'], []]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($url);

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
