--TEST--
UConverter::getDestinationEncoding()
--CREDITS--
Andy McNeice - PHP Testfest 2017
--EXTENSIONS--
intl
--FILE--
<?php
$c = new UConverter('UTF-7', 'ascii');
var_dump($c->getDestinationEncoding());
?>
--EXPECT--
string(5) "UTF-7"
