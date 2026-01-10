--TEST--
Cannot extend GMP class
--EXTENSIONS--
gmp
--FILE--
<?php

class T extends GMP {}

?>
--EXPECTF--
Fatal error: Class T cannot extend final class GMP in %s on line %d
