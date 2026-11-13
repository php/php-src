--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - multiple validation warnings
--EXTENSIONS--
reflection
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();

foreach ([
    "scheme" => "https",
    "host" => "127.0.0.1.",
    "path" => "\\foo",
] as $property => $value) {
    new ReflectionProperty($builder, $property)->setValue($builder, $value);
}

$url = $builder->build(errors: $errors);

var_dump($url->toAsciiString());
var_dump($url);
foreach ($errors as $error) {
    echo $error->type->name, ': ', $error->context, ': ';
    var_dump($error->failure);
}
var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString())));

?>
--EXPECTF--
string(21) "https://127.0.0.1/foo"
object(Uri\WhatWg\Url)#%d (%d) {
  ["scheme"]=>
  string(5) "https"
  ["username"]=>
  NULL
  ["password"]=>
  NULL
  ["host"]=>
  string(9) "127.0.0.1"
  ["port"]=>
  NULL
  ["path"]=>
  string(4) "/foo"
  ["query"]=>
  NULL
  ["fragment"]=>
  NULL
}
Ipv4EmptyPart: : bool(false)
InvalidReverseSoldius: \foo: bool(false)
bool(true)
