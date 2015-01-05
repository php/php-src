--TEST--
ldap_modify() - Modify operations that should fail
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

// Too few parameters
var_dump(ldap_modify());
var_dump(ldap_modify($link));
var_dump(ldap_modify($link, "dc=my-domain,dc=com"));

// Too many parameters
var_dump(ldap_modify($link, "dc=my-domain,dc=com", array(), "Additional data"));

// DN not found
var_dump(ldap_modify($link, "dc=my-domain,dc=com", array()));

// Invalid DN
var_dump(ldap_modify($link, "weirdAttribute=val", array()));

$entry = array(
	"objectClass"	=> array(
		"top",
		"dcObject",
		"organization"),
	"dc"			=> "my-domain",
	"o"				=> "my-domain",
);

ldap_add($link, "dc=my-domain,dc=com", $entry);

$entry2 = $entry;
$entry2["dc"] = "Wrong Domain";

var_dump(ldap_modify($link, "dc=my-domain,dc=com", $entry2));

$entry2 = $entry;
$entry2["weirdAttribute"] = "weirdVal";

var_dump(ldap_modify($link, "dc=my-domain,dc=com", $entry2));
?>
===DONE===
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

ldap_delete($link, "dc=my-domain,dc=com");
?>
--EXPECTF--
Warning: ldap_modify() expects exactly 3 parameters, 0 given in %s on line %d
NULL

Warning: ldap_modify() expects exactly 3 parameters, 1 given in %s on line %d
NULL

Warning: ldap_modify() expects exactly 3 parameters, 2 given in %s on line %d
NULL

Warning: ldap_modify() expects exactly 3 parameters, 4 given in %s on line %d
NULL

Warning: ldap_modify(): Modify: No such object in %s on line %d
bool(false)

Warning: ldap_modify(): Modify: Invalid DN syntax in %s on line %d
bool(false)

Warning: ldap_modify(): Modify: Naming violation in %s on line %d
bool(false)

Warning: ldap_modify(): Modify: Undefined attribute type in %s on line %d
bool(false)
===DONE===
