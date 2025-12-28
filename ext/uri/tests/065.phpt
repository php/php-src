--TEST--
Test that overwriting the URI is not possible
--EXTENSIONS--
uri
--FILE--
<?php

$uri = new Uri\WhatWg\Url('https://example.com');
try {
  $uri->__construct('ftp://example.org');
} catch (Throwable $e) {
  echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
var_dump($uri);

$uri = new Uri\WhatWg\Url('https://example.com');
try {
  $uri->__unserialize([['uri' => 'ftp://example.org'], []]);
} catch (Throwable $e) {
  echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
var_dump($uri);

$uri = new Uri\Rfc3986\Uri('https://example.com');
try {
  $uri->__construct('ftp://example.org');
} catch (Throwable $e) {
  echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
var_dump($uri);

$uri = new Uri\Rfc3986\Uri('https://example.com');
try {
  $uri->__unserialize([['uri' => 'ftp://example.org'], []]);
} catch (Throwable $e) {
  echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
var_dump($uri);

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
Error: Cannot modify readonly object of class Uri\Rfc3986\Uri
object(Uri\Rfc3986\Uri)#%d (8) {
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
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
Exception: Invalid serialization data for Uri\Rfc3986\Uri object
object(Uri\Rfc3986\Uri)#%d (8) {
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
  string(0) ""
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
