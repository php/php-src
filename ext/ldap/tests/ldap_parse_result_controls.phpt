--TEST--
ldap_parse_result() - Test the parsing of controls from result object
--CREDITS--
Côme Chilliet <mcmic@php.net>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
<?php
require_once('skipifcontrol.inc');
skipifunsupportedcontrol(LDAP_CONTROL_PAGEDRESULTS);
?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

$dn = "$base";
$filter = "(cn=user*)";
var_dump(
	ldap_control_paged_result($link, 1),
	$result = ldap_search($link, $dn, $filter, array('cn')),
	ldap_parse_result($link, $result, $errcode, $dn, $errmsg, $refs, $ctrls),
	$ctrls[LDAP_CONTROL_PAGEDRESULTS]['oid'],
	$ctrls[LDAP_CONTROL_PAGEDRESULTS]['value']['size'],
	bin2hex($ctrls[LDAP_CONTROL_PAGEDRESULTS]['value']['cookie']),
	ldap_get_entries($link, $result)['count']
);
?>
===DONE===
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
Deprecated: Function ldap_control_paged_result() is deprecated in %s.php on line %d
bool(true)
resource(%d) of type (ldap result)
bool(true)
string(22) "1.2.840.113556.1.4.319"
int(%d)
string(%d) "%s"
int(1)
===DONE===
