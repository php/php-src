--TEST--
GH-17398 (bcmul memory leak)
--EXTENSIONS--
bcmath
--FILE--
<?php
bcmul('0', '0', 2147483647);
?>
--EXPECTF--
Hard memory limit of 268435456 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d

Termsig=9
