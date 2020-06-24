--TEST--
Test gethostbyaddr() function : basic functionality
--FILE--
<?php
echo "*** Testing gethostbyaddr() : basic functionality ***\n";
echo gethostbyaddr("127.0.0.1")."\n";

?>
--EXPECTF--
*** Testing gethostbyaddr() : basic functionality ***
%rloopback|localhost(\.localdomain)?|%s%r
