--TEST--
UConverter::setSourceEncoding()
--CREDITS--
Andy McNeice - PHP Testfest 2017
--INI--
intl.error_level = E_WARNING
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
$c = new UConverter('latin1', 'ascii');
var_dump($c->getSourceEncoding());
$c->setSourceEncoding('utf-7');
var_dump($c->getSourceEncoding());
--EXPECT--
string(8) "US-ASCII"
string(5) "UTF-7"
