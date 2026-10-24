--TEST--
Test Uri\WhatWg\Url::parse() - success - file URL with invalid Windows drive letter warnings
--FILE--
<?php

$errors = [];
$baseUrl = new Uri\WhatWg\Url('file:///tmp/base');
$url = Uri\WhatWg\Url::parse('c|/foo', $baseUrl, $errors);

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
  string(4) "file"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(0) ""
  ["port"]=>
  NULL
  ["path"]=>
  string(7) "/c:/foo"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
string(14) "file:///c:/foo"
InvalidUrlUnit: |/foo: bool(false)
FileInvalidWindowsDriveLetter: c|/foo: bool(false)
