--TEST--
Check if static classes enforces static methods
--FILE--
<?php

static class A
{
    public function display($name)
    {
        echo "Hello $name\n";
    }
}

?>
--EXPECTF--
Fatal error: Class %s contains non-static method (%s) and therefore cannot be declared 'static' in %s on line %d