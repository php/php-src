--TEST--
dl() fails when trying to load full path to module
--CREDITS--
Tom Van Herreweghe <tom@theanalogguy.be>
User Group: PHP-WVL & PHPGent #PHPTestFest
--SKIPIF--
<?php
$enabled_sapi = array('cgi-fcgi', 'cli', 'embed', 'fpm');
if (!in_array(php_sapi_name(), $enabled_sapi)) {
	die('skip dl() is not enabled for ' . php_sapi_name());
}
?>
--INI--
enable_dl=1
--FILE--
<?php
var_dump(dl('/path/to/module'));
?>
--EXPECTF--

Warning: dl(): Temporary module name should contain only filename in %s on line %d
bool(false)
