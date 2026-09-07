--TEST--
ldap_count_entries() - Basic counting LDAP entries
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
insert_dummy_data($link, $base);
$result = ldap_search($link, "$base", "(objectclass=person)");
var_dump(
    ldap_count_entries($link, $result),
    count($result),
    $result->count(),
    // Let’s check that result is not changed by iterating
    $result->rewind(),
    $result->next(),
    $result->count(),
);
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECT--
int(3)
int(3)
int(3)
NULL
NULL
int(3)
