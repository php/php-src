--TEST--
UConverter::getDestinationEncoding()
--CREDITS--
Andy McNeice - PHP Testfest 2017
--INI--
intl.error_level = E_WARNING
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
$c = new UConverter('UTF-7', 'ascii');
var_dump($c->getDestinationEncoding());
?>
--EXPECT--
string(5) "UTF-7"
