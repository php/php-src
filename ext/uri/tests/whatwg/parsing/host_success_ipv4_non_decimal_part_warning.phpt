--TEST--
Test Uri\WhatWg\Url parsing - host - IPv4 non-decimal part warning
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse('http://0x1.2.3.4/', errors: $errors);

var_dump($url);
var_dump($url->toAsciiString());
foreach ($errors as $error) {
    echo $error->type->name, ': ', $error->context, ': ';
    var_dump($error->failure);
}

?>
--EXPECTF--
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(4) "http"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(7) "1.2.3.4"
  ["port"]=>
  NULL
  ["path"]=>
  string(1) "/"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(15) "http://1.2.3.4/"
Ipv4NonDecimalPart: /: bool(false)
