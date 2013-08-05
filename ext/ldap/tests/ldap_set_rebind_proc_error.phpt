--TEST--
ldap_set_rebind_proc() - Testing ldap_set_rebind_proc() that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php
	if (!function_exists("ldap_set_rebind_proc")) {
		die("skip ldap_set_rebind_proc() does not exist");
	}
	require "connect.inc";
	$link = fsockopen($host, $port);
	if (!$link) {
		die("skip no server listening");
	}
?>
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

$link = ldap_connect($host, $port);
var_dump(ldap_set_rebind_proc($link));
var_dump(ldap_set_rebind_proc($link, "rebind_proc", "Additional data"));
var_dump(ldap_set_rebind_proc($link, "rebind_proc_inexistent"));
?>
===DONE===
--EXPECTF--
Warning: ldap_set_rebind_proc() expects exactly 2 parameters, 1 given in %s on line %d
bool(false)

Warning: ldap_set_rebind_proc() expects exactly 2 parameters, 3 given in %s on line %d
bool(false)

Warning: ldap_set_rebind_proc(): Two arguments expected for 'rebind_proc_inexistent' to be a valid callback in %s on line %d
bool(false)
===DONE===
