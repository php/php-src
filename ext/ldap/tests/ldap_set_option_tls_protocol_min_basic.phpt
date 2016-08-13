--TEST--
ldap_set_option() - Basic test for TLS protocol min ldap option
--CREDITS--
Chad Sikorra <Chad.Sikorra@gmail.com>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";
$link = ldap_connect($host, $port);

foreach([
	LDAP_OPT_X_TLS_PROTOCOL_SSL2,
	LDAP_OPT_X_TLS_PROTOCOL_SSL3,
	LDAP_OPT_X_TLS_PROTOCOL_TLS1_0,
	LDAP_OPT_X_TLS_PROTOCOL_TLS1_1,
	LDAP_OPT_X_TLS_PROTOCOL_TLS1_2,
] as $option) {
	$result = ldap_set_option($link, LDAP_OPT_X_TLS_PROTOCOL_MIN, $option);
	var_dump($result);

	ldap_get_option($link, LDAP_OPT_X_TLS_PROTOCOL_MIN, $optionval);
	var_dump($optionval);
}
?>
===DONE===
--EXPECT--
bool(true)
int(512)
bool(true)
int(768)
bool(true)
int(769)
bool(true)
int(770)
bool(true)
int(771)
===DONE===
