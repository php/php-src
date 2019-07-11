--TEST--
Test ezmlm_hash() function : basic functionality
--FILE--
<?php
/* Prototype  : int ezmlm_hash  ( string $addr  )
 * Description: Calculate the hash value needed by EZMLM.
 * Source code: ext/standard/mail.c
 */

echo "*** Testing ezmlm_hash() : basic functionality ***\n";

var_dump(ezmlm_hash("webmaster@somewhere.com"));
var_dump(ezmlm_hash("foo@somewhere.com"));

?>
===Done===
--EXPECTF--
*** Testing ezmlm_hash() : basic functionality ***

Deprecated: Function ezmlm_hash() is deprecated in %s on line %d
int(1)

Deprecated: Function ezmlm_hash() is deprecated in %s on line %d
int(7)
===Done===
