--TEST--
Test Uri\WhatWg\Url parsing - path - invalid reverse solidus warning
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse('http://example.com\\foo', errors: $errors);

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
  string(11) "example.com"
  ["port"]=>
  NULL
  ["path"]=>
  string(4) "/foo"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(22) "http://example.com/foo"
InvalidReverseSoldius: \foo: bool(false)
