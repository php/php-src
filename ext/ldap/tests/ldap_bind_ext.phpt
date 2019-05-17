--TEST--
ldap_bind_ext() - Basic binding
--CREDITS--
Côme Chilliet <mcmic@php.net>
--SKIPIF--
<?php require_once __DIR__ .'/skipif.inc'; ?>
<?php require_once __DIR__ .'/skipifbindfailure.inc'; ?>
<?php
require_once('skipifcontrol.inc');
skipifunsupportedcontrol(LDAP_CONTROL_PASSWORDPOLICYREQUEST);
?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);

var_dump(
	$result = ldap_bind_ext($link, $user, $passwd),
	ldap_parse_result($link, $result, $errcode, $matcheddn, $errmsg, $referrals, $ctrls),
	$errcode,
	$errmsg,
	$ctrls,
	$result = ldap_bind_ext($link, $user, $passwd, [['oid' => LDAP_CONTROL_PASSWORDPOLICYREQUEST]]),
	ldap_parse_result($link, $result, $errcode, $matcheddn, $errmsg, $referrals, $ctrls),
	$errcode,
	$errmsg,
	$ctrls
);

/* Failures */
var_dump(
	$result = ldap_bind_ext($link, $user, "wrongPassword", [['oid' => LDAP_CONTROL_PASSWORDPOLICYREQUEST]]),
	ldap_parse_result($link, $result, $errcode, $matcheddn, $errmsg, $referrals, $ctrls),
	$errcode,
	$errmsg,
	$ctrls,
	$result = ldap_bind_ext($link, "unexistingProperty=weirdValue,$user", $passwd, [['oid' => LDAP_CONTROL_PASSWORDPOLICYREQUEST]]),
	ldap_parse_result($link, $result, $errcode, $matcheddn, $errmsg, $referrals, $ctrls),
	$errcode,
	$errmsg,
	$ctrls
);
?>
===DONE===
--EXPECTF--
resource(%d) of type (ldap result)
bool(true)
int(0)
string(0) ""
array(0) {
}
resource(%d) of type (ldap result)
bool(true)
int(0)
string(0) ""
array(0) {
}
resource(%d) of type (ldap result)
bool(true)
int(49)
string(0) ""
array(0) {
}
resource(%d) of type (ldap result)
bool(true)
int(34)
string(10) "invalid DN"
array(0) {
}
===DONE===
