--TEST--
ldap_modify_batch() - Batch modify operations that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
Ondřej Hošek <ondra.hosek@gmail.com>
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);

$addGivenName = array(
    array(
        "attrib"	=> "givenName",
        "modtype"	=> LDAP_MODIFY_BATCH_ADD,
        "values"	=> array("Jack")
    )
);

// DN not found
var_dump(ldap_modify_batch($link, "cn=not-found,$base", $addGivenName));

// Invalid DN
var_dump(ldap_modify_batch($link, "weirdAttribute=val", $addGivenName));

// prepare
$entry = array(
    "objectClass"	=> array(
        "top",
        "dcObject",
        "organization"),
    "dc"			=> "my-domain",
    "o"				=> "my-domain",
);

ldap_add($link, "dc=my-domain,$base", $entry);

// invalid domain
$mods = array(
    array(
        "attrib"	=> "dc",
        "modtype"	=> LDAP_MODIFY_BATCH_REPLACE,
        "values"	=> array("Wrong Domain")
    )
);

var_dump(ldap_modify_batch($link, "dc=my-domain,$base", $mods));

// invalid attribute
$mods = array(
    array(
        "attrib"	=> "weirdAttribute",
        "modtype"	=> LDAP_MODIFY_BATCH_ADD,
        "values"	=> array("weirdVal", "anotherWeirdval")
    )
);

var_dump(ldap_modify_batch($link, "dc=my-domain,$base", $mods));
?>
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);

ldap_delete($link, "dc=my-domain,$base");
?>
--EXPECTF--
Warning: ldap_modify_batch(): Batch Modify: No such object in %s on line %d
bool(false)

Warning: ldap_modify_batch(): Batch Modify: Invalid DN syntax in %s on line %d
bool(false)

Warning: ldap_modify_batch(): Batch Modify: Naming violation in %s on line %d
bool(false)

Warning: ldap_modify_batch(): Batch Modify: Undefined attribute type in %s on line %d
bool(false)
