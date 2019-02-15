--TEST--
dl() returns false when disabled via INI settings
+--CREDITS--
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
enable_dl=0
--FILE--
<?php
var_dump(dl('foo'));
?>
--EXPECTF--

Warning: dl(): Dynamically loaded extensions aren't enabled in %s on line %d
bool(false)
