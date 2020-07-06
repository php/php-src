--TEST--
Bug #61025 (__invoke() visibility not honored)
--FILE--
<?php

class Bar {
    private function __invoke() {
        return __CLASS__;
    }
}

$b = new Bar;
echo $b();

echo $b->__invoke();

?>
--EXPECTF--
Warning: Method Bar::__invoke() must have public visibility in %sbug61025.php on line %d
Bar
Fatal error: Uncaught Error: Private method Bar::__invoke() cannot be called from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %sbug61025.php on line %d
