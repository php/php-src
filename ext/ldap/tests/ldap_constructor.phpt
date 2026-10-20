--TEST--
Attempt to instantiate an LDAP\Connection directly
--EXTENSIONS--
ldap
--FILE--
<?php

try {
    new LDAP\Connection();
} catch (Error $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
?>
--EXPECT--
Error: Cannot directly construct LDAP\Connection, use ldap_connect() instead
