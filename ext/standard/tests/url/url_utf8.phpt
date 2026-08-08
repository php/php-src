--TEST--
Uri: hostnames should be preserved in Unicode form
--FILE--
<?php

$parsed = parse_url('http://ουτοπία.δπθ.gr/');
var_dump($parsed['host']);
?>
--EXPECT--
string(24) "ουτοπία.δπθ.gr"