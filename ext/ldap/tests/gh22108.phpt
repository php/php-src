--TEST--
GH-22108 (Integral float values are rejected when modifying LDAP entries)
--EXTENSIONS--
ldap
--SKIPIF--
<?php require_once('skipifbindfailure.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);

var_dump(ldap_mod_replace($link, "cn=userA,$base", [
    "description" => [round(20400.4), -3.0, -0.0, 1e20],
]));

$entries = ldap_get_entries(
    $link,
    ldap_read($link, "cn=userA,$base", "(objectClass=*)", ["description"])
);
$values = $entries[0]["description"];
unset($values["count"]);
sort($values);
var_dump($values);

foreach ([1.5, NAN, INF] as $value) {
    try {
        ldap_mod_replace($link, "cn=userA,$base", ["description" => $value]);
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--CLEAN--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);

remove_dummy_data($link, $base);
?>
--EXPECT--
bool(true)
array(4) {
  [0]=>
  string(2) "-3"
  [1]=>
  string(1) "0"
  [2]=>
  string(5) "20400"
  [3]=>
  string(21) "100000000000000000000"
}
LDAP value must be of type string|int|bool, float given
LDAP value must be of type string|int|bool, float given
LDAP value must be of type string|int|bool, float given
