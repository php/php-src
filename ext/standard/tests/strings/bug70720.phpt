--TEST--
Bug #70720 (strip_tags() doesnt handle "xml" correctly)
--FILE--
<?php
var_dump(strip_tags('<?php $dom->test(); ?> this is a test'));
var_dump(strip_tags('<?php $xml->test(); ?> this is a test'));
var_dump(strip_tags('<?xml $xml->test(); ?> this is a test'));

/* "->" case in HTML */
var_dump(strip_tags("<span class=sf-dump-> this is a test</span>"));
?>
--EXPECT--
string(15) " this is a test"
string(15) " this is a test"
string(15) " this is a test"
string(15) " this is a test"
