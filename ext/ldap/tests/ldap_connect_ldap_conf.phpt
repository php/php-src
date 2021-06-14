--TEST--
ldap_connect() - Connection using default host from openldap's ldap.conf
--CREDITS--
David Caldwell <david@galvanix.com>
--EXTENSIONS--
ldap
--ENV--
LDAPCONF={PWD}/ldap_connect_ldap_conf.conf
--SKIPIF--
<?php
ob_start();
phpinfo();
if (!preg_match("/vendor name => openldap/i", ob_get_clean()))
    die("skip not openldap");
?>
--FILE--
<?php
$link = ldap_connect();
ldap_get_option($link, LDAP_OPT_HOST_NAME, $host);
var_dump($host);
?>
--EXPECT--
string(16) "example.com:3141"
