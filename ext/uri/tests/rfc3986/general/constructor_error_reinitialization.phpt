--TEST--
Test Uri\Rfc3986\Uri __construct() - error - reinitialization
--FILE--
<?php

$uri = new Uri\Rfc3986\Uri('https://example.com');

try {
    $uri->__construct('ftp://example.org');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($uri);
var_dump($uri->toRawString());
var_dump($uri->toString());

?>
--EXPECTF--
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
string(19) "https://example.com"
string(19) "https://example.com"
