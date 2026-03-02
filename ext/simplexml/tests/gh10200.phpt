--TEST--
GH-10200 (zif_get_object_vars: Assertion `!(((__ht)->u.flags & (1<<2)) != 0)' failed.)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xmlData = <<<EOF
<?xml version="1.0" encoding="utf-8"?>
<document>https://github.com/php/php-src/issues/10200 not encountered</document>
EOF;

$xml = simplexml_load_string($xmlData);
$output = get_object_vars($xml);
var_dump($output);

?>
--EXPECT--
array(1) {
  [0]=>
  string(59) "https://github.com/php/php-src/issues/10200 not encountered"
}
