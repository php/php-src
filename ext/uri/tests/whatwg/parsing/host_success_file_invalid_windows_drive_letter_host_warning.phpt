--TEST--
Test Uri\WhatWg\Url parsing - host - file invalid Windows drive letter host warning
--FILE--
<?php

$errors = [];
$url = Uri\WhatWg\Url::parse('file://c:/foo', errors: $errors);

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
FileInvalidWindowsDriveLetterHost: c:/foo: bool(false)
