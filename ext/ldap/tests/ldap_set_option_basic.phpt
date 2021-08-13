--TEST--
ldap_set_option() - Basic ldap_set_option() operation
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
$option = null;

var_dump(ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version));
ldap_get_option($link, LDAP_OPT_PROTOCOL_VERSION, $option);
var_dump($option);
?>
--EXPECT--
bool(true)
int(3)
