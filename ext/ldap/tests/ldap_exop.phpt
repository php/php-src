--TEST--
ldap_exop() and ldap_parse_exop() - EXOP operations
--CREDITS--
Côme Chilliet <mcmic@php.net>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

function ber_encode($string) {
	$null_padded_string = '';
	for($i = 0; $i<strlen($string); $i++) {
		$null_padded_string .= substr($string,$i,1)."\0";
	}
	return base64_encode($null_padded_string);
}

// ldap_exop(resource link, string reqoid [, string reqdata [, string retoid [, string retdata]]])
// bool ldap_parse_exop(resource link, resource result [, string retoid [, string retdata]])
var_dump(
	ldap_exop($link, "1.3.6.1.4.1.4203.1.11.3", NULL, $retoid, $retdata),
	$retdata,
	$retoid,
	$r = ldap_exop($link, "1.3.6.1.4.1.4203.1.11.3"),
	ldap_parse_exop($link, $r, $retoid2, $retdata2),
	$retdata2,
	$retoid2
);
?>
===DONE===
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

remove_dummy_data($link, $base);
?>
--EXPECTF--
bool(true)
string(%d) "dn:%s"
string(0) ""
resource(%d) of type (ldap result)
bool(true)
string(%d) "dn:%s"
string(0) ""
===DONE===
