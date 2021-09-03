--TEST--
Bug #62443 Crypt SHA256/512 Segfaults With Malformed Salt
--FILE--
<?php
crypt("foo", '$5$'.chr(0).'abc');
crypt("foo", '$6$'.chr(0).'abc');
echo "OK!";
?>
--EXPECT--
OK!
