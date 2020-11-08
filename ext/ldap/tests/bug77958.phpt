--TEST--
ldap_modify_batch() - bug 77958 - values in ldap_modify_batch must be "string"
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

$mods = array(
    array(
        "attrib"	=> "telephoneNumber",
        "modtype"	=> LDAP_MODIFY_BATCH_ADD,
        "values"	=> array(
            123456
        )
    ),
    array(
        "attrib"	=> "description",
        "modtype"	=> LDAP_MODIFY_BATCH_REMOVE_ALL
    )
);

var_dump(
    ldap_modify_batch($link, "cn=userA,$base", $mods),
    $entry = ldap_first_entry($link, ldap_read($link, "cn=userA,$base", "(telephoneNumber=*)")),
    ldap_get_values($link, $entry, "telephoneNumber")
);
?>
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($host, $port, $user, $passwd, $protocol_version);

remove_dummy_data($link, $base);
?>
--EXPECTF--
bool(true)
resource(%d) of type (ldap result entry)
array(3) {
  [0]=>
  string(14) "xx-xx-xx-xx-xx"
  [1]=>
  string(6) "123456"
  ["count"]=>
  int(2)
}
