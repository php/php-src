--TEST--
`never` parameter types - allowed on interfaces' methods
--FILE--
<?php

interface Demo {
    public function example(never $v);
}

?>
--EXPECT--
