--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - collects and resets soft errors with a base URL
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setPath("/a\tb");
$builder->setFragment("x\ny");
$base = new Uri\WhatWg\Url("https://example.com/");
$softErrors = ["old"];
$url = $builder->build($base, $softErrors);

var_dump($url->toAsciiString());
foreach ($softErrors as $error) {
    var_dump($error->type);
}

$builder->setPath("/ab");
$builder->setFragment("xy");
$builder->build($base, $softErrors);
var_dump($softErrors);

?>
--EXPECT--
string(25) "https://example.com/ab#xy"
enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
array(0) {
}
