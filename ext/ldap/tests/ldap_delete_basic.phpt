--TEST--
ldap_delete() - Basic delete operation
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
ldap_add($link, "dc=my-domain,dc=com", array(
	"objectClass"	=> array(
		"top",
		"dcObject",
		"organization"),
	"dc"			=> "my-domain",
	"o"				=> "my-domain",
));

var_dump(
	ldap_delete($link, "dc=my-domain,dc=com"),
	@ldap_search($link, "dc=my-domain,dc=com", "(o=my-domain)")
);
?>
===DONE===
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

ldap_delete($link, "dc=my-domain,dc=com");
?>
--EXPECT--
bool(true)
bool(false)
===DONE===
