--TEST--
Bug #61025 (__invoke() visibility not honored)
--FILE--
<?php

Interface InvokeAble {
    static function __invoke();
}

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
Warning: The magic method __invoke() must have public visibility and cannot be static in %sbug61025.php on line %d

Warning: The magic method __invoke() must have public visibility and cannot be static in %sbug61025.php on line %d
Bar
Fatal error: Call to private method Bar::__invoke() from context '' in %sbug61025.php on line %d
