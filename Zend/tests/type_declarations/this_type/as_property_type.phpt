--TEST--
Cannot use $this as property type
--FILE--
<?php

class Test {
    public $this $prop;
}

?>
--EXPECTF--
Parse error: syntax error, unexpected variable "$prop", expecting "," or ";" in %s on line %d
