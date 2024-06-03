--TEST--
Test manual Uri instance creation error cases
--EXTENSIONS--
uri
--FILE--
<?php

$url = new Uri\Uri("https", "example.com", null, null, null, null, null, null);

var_dump($url);

try {
    $url->__construct("https", "example.com", 8080, "user", "password", "/path", "q=r", "fragment");
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
object(Uri\Uri)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(11) "example.com"
  ["port"]=>
  NULL
  ["user"]=>
  NULL
  ["password"]=>
  NULL
  ["path"]=>
  NULL
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
Cannot modify readonly property Uri\Uri::$scheme
