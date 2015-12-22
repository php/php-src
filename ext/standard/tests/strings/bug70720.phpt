--TEST--
Bug #70720 (strip_tags() doesnt handle "xml" correctly)
--FILE--
<?php
var_dump(strip_tags('<?php $dom->test(); ?> this is a test'));
var_dump(strip_tags('<?php $xml->test(); ?> this is a test'));
var_dump(strip_tags('<?xml $xml->test(); ?> this is a test'));
?>
--EXPECTF--
string(15) " this is a test"
string(15) " this is a test"
string(15) " this is a test"