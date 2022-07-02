--TEST--
ldap_modify() - Modify operations that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

// DN not found
var_dump(ldap_modify($link, "cn=not-found,$base", array()));

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

ldap_add($link, "dc=my-domain,$base", $entry);

$entry2 = $entry;
$entry2["dc"] = "Wrong Domain";

var_dump(ldap_modify($link, "dc=my-domain,$base", $entry2));

$entry2 = $entry;
$entry2["weirdAttribute"] = "weirdVal";

var_dump(ldap_modify($link, "dc=my-domain,$base", $entry2));
?>
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

ldap_delete($link, "dc=my-domain,$base");
?>
--EXPECTF--
Warning: ldap_modify(): Modify: No such object in %s on line %d
bool(false)

Warning: ldap_modify(): Modify: Invalid DN syntax in %s on line %d
bool(false)

Warning: ldap_modify(): Modify: Naming violation in %s on line %d
bool(false)

Warning: ldap_modify(): Modify: Undefined attribute type in %s on line %d
bool(false)
