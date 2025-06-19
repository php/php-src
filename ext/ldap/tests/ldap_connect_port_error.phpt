--TEST--
ldap_connect() - Connection errors
--EXTENSIONS--
ldap
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
require "connect.inc";
try {
	ldap_connect("nope://$host", 65536);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}

try {
	ldap_connect("nope://$host", 0);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ldap_connect(): Argument #2 ($port) must be between 1 and 65535
ldap_connect(): Argument #2 ($port) must be between 1 and 65535
