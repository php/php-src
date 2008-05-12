--TEST--
Testing interface declaration using the original and alias class name
--FILE--
<?php

interface a { }

class_alias('a', 'b');

interface c extends a, b { }

?>
--EXPECTF--
Fatal error: Class c cannot implement previously implemented interface a in %s on line %d
