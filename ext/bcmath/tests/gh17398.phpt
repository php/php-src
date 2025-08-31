--TEST--
GH-17398 (bcmul memory leak)
--EXTENSIONS--
bcmath
--FILE--
<?php
bcmul('0', '0', 2147483647);
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
