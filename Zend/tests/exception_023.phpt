--TEST--
Ensure proper backtraces with anon classes
--FILE--
<?php

(function($obj) {
    throw new Exception();
})(new class {});

?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): {closure}(Object(class@anonymous))
#1 {main}
  thrown in %s on line %d
