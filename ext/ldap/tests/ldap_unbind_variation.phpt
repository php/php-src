--TEST--
ldap_unbind() - Variation of ldap_unbind() function using ldap_set_rebind_proc()
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

function rebind_proc ($ds, $ldap_url) {
	global $user;
	global $passwd;
	global $protocol_version;
	
	// required by most modern LDAP servers, use LDAPv3
	ldap_set_option($a, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
	
	if (!ldap_bind($a, $user, $passwd)) {
		print "Cannot bind";
	}
}

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
ldap_set_rebind_proc($link, "rebind_proc");

var_dump(ldap_unbind($link));
?>
===DONE===
--EXPECT--
bool(true)
===DONE===
