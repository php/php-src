--TEST--
Test posix_getsid() function : error conditions
--DESCRIPTION--
cases: no params, wrong param, wrong param range
--CREDITS--
Moritz Neuhaeuser, info@xcompile.net
PHP Testfest Berlin 2009-05-10
--EXTENSIONS--
posix
--FILE--
<?php
var_dump( posix_getsid(-1) );
?>
--EXPECT--
bool(false)
