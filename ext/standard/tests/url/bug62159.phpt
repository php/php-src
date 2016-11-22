--TEST--
Bug #62159 (All ports greater than 65535 in parse_url)
--FILE--
<?php
$url = 'ctpp://ericsson:78325/?carrier=31003&address=2125551212';
$parsedURL = parse_url($url);
var_dump($parsedURL);
--EXPECT--
array(5) {
  ["scheme"]=>
  string(4) "ctpp"
  ["host"]=>
  string(8) "ericsson"
  ["port"]=>
  int(78325)
  ["path"]=>
  string(1) "/"
  ["query"]=>
  string(32) "carrier=31003&address=2125551212"
}
