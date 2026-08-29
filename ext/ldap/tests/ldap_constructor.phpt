--TEST--
Attempt to instantiate an LDAP\Connection directly
--EXTENSIONS--
ldap
--FILE--
<?php

$ldap = new LDAP\Connection();
var_dump($ldap);
?>
--EXPECTF--
object(LDAP\Connection)#%d (0) {
}
