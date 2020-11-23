--TEST--
ldap_first_attribute() - Testing ldap_first_attribute() that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($host, $port);
try {
    var_dump(ldap_first_attribute($link, $link));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
ldap_first_attribute(): supplied resource is not a valid ldap result entry resource
