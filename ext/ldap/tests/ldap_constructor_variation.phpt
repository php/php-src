--TEST--
new \LDAP\Connection() - Variation
--CREDITS--
Côme Chilliet <mcmic@php.net>
--EXTENSIONS--
ldap
--FILE--
<?php
require "connect.inc";

// no hostname, no port
$link = new \LDAP\Connection();
var_dump($link);
ldap_get_option($link, LDAP_OPT_HOST_NAME, $hostname);
var_dump($hostname);

// URI
$link = new \LDAP\Connection("ldap://hostname:389");
var_dump($link);
ldap_get_option($link, LDAP_OPT_HOST_NAME, $hostname);
var_dump($hostname);

// ldaps URI
$link = new \LDAP\Connection("ldaps://hostname:689");
var_dump($link);
ldap_get_option($link, LDAP_OPT_HOST_NAME, $hostname);
var_dump($hostname);

// URI no port
$link = new \LDAP\Connection("ldap://hostname");
var_dump($link);
ldap_get_option($link, LDAP_OPT_HOST_NAME, $hostname);
var_dump($hostname);
?>
--EXPECTF--
object(LDAP\Connection)#%d (0) {
}
string(%d) "%s:%d"
object(LDAP\Connection)#%d (0) {
}
string(12) "hostname:389"
object(LDAP\Connection)#%d (0) {
}
string(12) "hostname:689"
object(LDAP\Connection)#%d (0) {
}
string(12) "hostname:389"
