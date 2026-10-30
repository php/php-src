--TEST--
GH-17704 (ldap_search fails when $attributes contains a non-packed array with numerical keys)
--EXTENSIONS--
ldap
--SKIPIF--
<?php
require_once('skipifbindfailure.inc');
?>
--FILE--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);

$attributes = ["uid", "cn"];
// force a non-packed array
$attributes["x"] = "x";
unset($attributes["x"]);

var_dump(ldap_search($link, $base, "(o=*)", $attributes));
?>
--EXPECTF--
object(LDAP\Result)#%d (0) {
}
