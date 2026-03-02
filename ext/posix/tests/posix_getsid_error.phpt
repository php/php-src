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
try {
	posix_getsid(-1);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
posix_getsid(): Argument #1 ($process_id) must be between 0 and %d
