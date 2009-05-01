--TEST--
Testing Basic behaviour of strcoll()
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich
--XFAIL--
Needs a fix : http://news.php.net/php.qa/64836
--FILE--
<?php

 $a = 'a';
 $b = 'A';

 locale_set_default('en_US');
$result = strcoll($a, $b);
if($result > 0) {
	echo "Pass\n";
}
?>
--EXPECT--
Pass

