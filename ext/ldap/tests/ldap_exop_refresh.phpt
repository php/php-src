--TEST--
ldap_exop_refresh() - Test LDAP refresh extended operation
--CREDITS--
Emmanuel Dreyfus <manu@netbsd.org>
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php require_once('skipifbindfailure.inc'); ?>
<?php
	$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
	$r = ldap_read($link, '', 'objectClass=*', array('dynamicsubtrees'));
	$info = ldap_get_entries($link, $r)[0];
	if (!isset($info['dynamicsubtrees'])) {
		die("skip Overlay DDS not available");
	}
?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

insert_dummy_data($link, $base);
ldap_add($link, "cn=tmp,$base", array(
	"objectclass" => array("person", "dynamicObject"),
	"cn" => "tmp",
	"sn" => "tmp"
));
var_dump(
	ldap_exop_refresh($link, "cn=tmp,$base", 1234)
);
?>
===DONE===
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);
ldap_delete($link, "cn=tmp,$base");
remove_dummy_data($link, $base);
?>
--EXPECT--
int(1234)
===DONE===
