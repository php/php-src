--TEST--
ldap_sasl_bind() - Basic anonymous binding
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--SKIPIF--
<?php if (!function_exists("ldap_sasl_bind")) die("skip LDAP extension not compiled with SASL support"); ?>
<?php
    require "connect.inc";
    $link = @fsockopen($uri);
    if (!$link) {
        die("skip no server listening");
    }
?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
insert_dummy_data($link, $base);
ldap_unbind($link);

$link = ldap_connect($uri);
ldap_set_option($link, LDAP_OPT_PROTOCOL_VERSION, $protocol_version);
var_dump(ldap_sasl_bind($link, null, $sasl_passwd, 'DIGEST-MD5', 'realm', $sasl_user));
?>
--CLEAN--
<?php
include "connect.inc";

$link = ldap_connect_and_bind($uri, $user, $passwd, $protocol_version);
remove_dummy_data($link, $base);
?>
--EXPECT--
bool(true)
