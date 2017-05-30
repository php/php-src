--TEST--
ldap_connect() - Connection using default host from openldap's ldap.conf
--CREDITS--
David Caldwell <david@galvanix.com>
--SKIPIF--
<?php
require_once('skipif.inc');
ob_start();
phpinfo();
if (!preg_match("/vendor name => openldap/i", ob_get_clean()))
    die("skip not openldap");
?>
--FILE--
<?php
$conf=tempnam("/tmp","ldap.conf");
file_put_contents($conf, "uri ldaps://example.com:3141/");
putenv("LDAPCONF=$conf");
$link = ldap_connect();
ldap_get_option($link, LDAP_OPT_HOST_NAME, $host);
var_dump($host);
unlink($conf);
?>
===DONE===
--EXPECTF--
string(16) "example.com:3141"
===DONE===
