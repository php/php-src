--TEST--
Test Uri\WhatWg\Url::withPath() - file URL whose path was reset by an invalid drive letter
--FILE--
<?php

$url = Uri\WhatWg\Url::parse("c|/x", new Uri\WhatWg\Url("file:///d:/a/b"), $errors);

var_dump($url->getPath());
var_dump(array_map(static fn (Uri\WhatWg\UrlValidationError $error): string => $error->type->name, $errors));
var_dump($url->withPath("/zz")->getPath());

?>
--EXPECT--
string(5) "/c:/x"
array(2) {
  [0]=>
  string(14) "InvalidUrlUnit"
  [1]=>
  string(29) "FileInvalidWindowsDriveLetter"
}
string(3) "/zz"
