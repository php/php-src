--TEST--
Check if static classes enforces static properties
--FILE--
<?php

static class A
{
    public $foo = 'bar';
}

?>
--EXPECTF--
Fatal error: Class %s contains non-static property declaration and therefore cannot be declared 'static' in %s on line %d
