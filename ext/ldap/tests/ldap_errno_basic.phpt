--TEST--
ldap_errno() - Basic ldap_errno() operation
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

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
@ldap_add($link, "badDN $base", array(
    "objectClass"	=> array(
        "top",
        "dcObject",
        "organization"),
    "dc"			=> "my-domain",
    "o"				=> "my-domain",
));

var_dump(
    ldap_errno($link)
);
?>
--EXPECT--
int(34)
