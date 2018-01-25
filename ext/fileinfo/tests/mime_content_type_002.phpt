--TEST--
mime_content_type(): Testing parameter
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

var_dump(mime_content_type(__FILE__));
var_dump(mime_content_type(fopen(__FILE__, 'r')));
var_dump(mime_content_type('.'));
var_dump(mime_content_type('./..'));

?>
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
string(9) "directory"
string(9) "directory"
