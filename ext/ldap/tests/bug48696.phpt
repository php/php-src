--TEST--
Bug #48696 (ldap_read() segfaults with invalid parameters)
--SKIPIF--
<?php
require_once dirname(__FILE__) .'/skipif.inc';
?>
--FILE--
<?php

ldap_read(1,1,1);

?>
--EXPECTF--
Warning: ldap_read(): supplied argument is not a valid ldap link resource in %s on line %d
