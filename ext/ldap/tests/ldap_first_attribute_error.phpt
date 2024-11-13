--TEST--
ldap_first_attribute() - Testing ldap_first_attribute() that should fail
--CREDITS--
Patrick Allaert <patrickallaert@php.net>
# Belgian PHP Testfest 2009
--EXTENSIONS--
ldap
--FILE--
<?php
require "connect.inc";

$link = ldap_connect($uri);
try {
    var_dump(ldap_first_attribute($link, $link));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
ldap_first_attribute(): Argument #2 ($entry) must be of type LDAP\ResultEntry, LDAP\Connection given
