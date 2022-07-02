--TEST--
Bug #72021 (ldap_escape() with DN flag is not RFC compliant)
--CREDITS--
Chad Sikorra <Chad.Sikorra@gmail.com>
--EXTENSIONS--
ldap
--FILE--
<?php
$subject = " Joe,= \rSmith ";

var_dump(ldap_escape($subject, '', LDAP_ESCAPE_DN));
?>
--EXPECT--
string(24) "\20Joe\2c\3d \0dSmith\20"
