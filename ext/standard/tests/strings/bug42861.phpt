--TEST--
Bug #42861 (strtr() crashes in Unicode mode when $from argument is empty)
--FILE--
<?php

var_dump( strtr("hello", array("" => "string") ) );
var_dump( strtr("hello", array('' => "string") ) );
var_dump( strtr("hello", array(null => "string") ) );
var_dump( strtr("hello", array(NULL => "string") ) );

var_dump( strtr("hello", "", "string") );
var_dump( strtr("hello", '', "string") );
var_dump( strtr("hello", NULL, "string") );
var_dump( strtr("hello", null, "string") );

echo "Done\n";
?>
--EXPECT--
unicode(5) "hello"
unicode(5) "hello"
unicode(5) "hello"
unicode(5) "hello"
unicode(5) "hello"
unicode(5) "hello"
unicode(5) "hello"
unicode(5) "hello"
Done
