--TEST--
Cannot use $this as param type
--FILE--
<?php

class Test {
    public function method($this $param) {}
}

?>
--EXPECTF--
Parse error: syntax error, unexpected variable "$param", expecting ")" in %s on line %d
