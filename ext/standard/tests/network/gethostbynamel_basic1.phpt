--TEST--
Test gethostbynamel() function : basic functionality 
--FILE--
<?php
/* Prototype  : array gethostbynamel  ( string $hostname  )
 * Description: Get a list of IPv4 addresses corresponding to a given Internet host name 
 * Source code: ext/standard/dns.c
*/

echo "*** Testing gethostbynamel() : basic functionality ***\n";
var_dump(gethostbynamel("localhost"));
?>
===DONE===
--EXPECTF--
*** Testing gethostbynamel() : basic functionality ***
array(%d) {
  %a
}
===DONE===