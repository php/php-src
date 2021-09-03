--TEST--
Bug #60909 (custom error handler throwing Exception + fatal error = no shutdown function).
--FILE--
<?php
register_shutdown_function(function(){echo("\n\n!!!shutdown!!!\n\n");});

class Bad {
    public function __toString() {
        throw new Exception('I CAN DO THIS');
    }
}

$bad = new Bad();
echo "$bad";
?>
--EXPECTF--
Fatal error: Uncaught Exception: I CAN DO THIS in %s:%d
Stack trace:
#0 %s(%d): Bad->__toString()
#1 {main}
  thrown in %s on line %d


!!!shutdown!!!
