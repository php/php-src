--TEST--
return type with finally
--FILE--
<?php

function foo() : array {
    try {
        throw new Exception("xxxx");
    } finally {
        return null;
    }
}

foo();
?>
--EXPECTF--
Fatal error: Uncaught Exception: xxxx in %s:%d
Stack trace:
#0 %s(%d): foo()
#1 {main}

Next TypeError: foo(): Return value must be of type array, null returned in %s:%d
Stack trace:
#0 %s(%d): foo()
#1 {main}
  thrown in %s029.php on line %d
