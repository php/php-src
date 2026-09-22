--TEST--
BcMath\Number class extends error
--EXTENSIONS--
bcmath
--FILE--
<?php

class Child extends \BcMath\Number{}

$child = new Child(1);
?>
--EXPECTF--
Fatal error: Class Child cannot extend final class BcMath\Number in %s
