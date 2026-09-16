--TEST--
UConverter::getSourceEncoding()
--CREDITS--
Andy McNeice - PHP Testfest 2017
--EXTENSIONS--
intl
--FILE--
<?php
$c = new UConverter('utf-32', 'ascii');
var_dump($c->getSourceEncoding());
?>
--EXPECT--
string(8) "US-ASCII"
