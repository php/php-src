--TEST--
ldap_free_result() - entries of a freed result can no longer be used
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

$result = ldap_search($link, "cn=userA,$base", "(cn=*)");
$entry = ldap_first_entry($link, $result);
var_dump(ldap_get_dn($link, $entry));
ldap_free_result($result);

foreach ([
    fn() => ldap_get_dn($link, $entry),
    fn() => ldap_get_attributes($link, $entry),
    fn() => ldap_first_attribute($link, $entry),
    fn() => ldap_next_attribute($link, $entry),
    fn() => ldap_next_entry($link, $entry),
    fn() => ldap_get_values_len($link, $entry, "cn"),
] as $fn) {
    try {
        $fn();
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECTF--
string(%d) "cn=userA,%s"
Error: LDAP result has already been closed
Error: LDAP result has already been closed
Error: LDAP result has already been closed
Error: LDAP result has already been closed
Error: LDAP result has already been closed
Error: LDAP result has already been closed
