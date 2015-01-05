--TEST--
Test gethostbyname() function : basic functionality 
--FILE--
<?php
/* Prototype  : string gethostbyname  ( string $hostname  )
 * Description: Get the IPv4 address corresponding to a given Internet host name 
 * Source code: ext/standard/dns.c
*/

echo "*** Testing gethostbyname() : basic functionality ***\n";

echo gethostbyname("localhost")."\n";
?>
===DONE===
--EXPECT--
*** Testing gethostbyname() : basic functionality ***
127.0.0.1
===DONE===