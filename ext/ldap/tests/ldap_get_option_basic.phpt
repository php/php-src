--TEST--
ldap_get_option() - Basic ldap_get_option() operation
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($uri);
$option = null;
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);

var_dump(
    ldap_get_option($link, LDAP_OPT_PROTOCOL_VERSION, $option),
    $option
);
?>
--EXPECT--
bool(true)
int(3)
