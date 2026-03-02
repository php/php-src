--TEST--
Bug #38461 (setting private attribute with __set() produces segfault)
--FILE--
<?php

class Operation
{
    function __set( $var, $value )
    {
        $this->$var = $value;
    }
}

class ExtOperation extends Operation
{
    private $x;
}

$op = new ExtOperation;
$op->x = 'test';

echo "Done\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot access private property ExtOperation::$x in %s:%d
Stack trace:
#0 %s(%d): Operation->__set('x', 'test')
#1 {main}
  thrown in %s on line %d
