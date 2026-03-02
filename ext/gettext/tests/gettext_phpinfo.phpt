--TEST--
Test phpinfo() displays gettext support
--EXTENSIONS--
gettext
--FILE--
<?php
phpinfo();
?>
--EXPECTF--
%a
%rGetText Support.*enabled%r
%a
--CREDITS--
Tim Eggert, tim@elbart.com
PHP Testfest Berlin 2009-05-09
