--TEST--
Turbofish bound check: method call with a concrete arg violating the bound
--FILE--
<?php
class Animal {}
class Service {
    public function pick<T : Animal>(): void {}
    public static function spick<T : Animal>(): void {}
}

try {
    (new Service)->pick::<int>();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    Service::spick::<int>();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Type argument 1 to call Service::pick() does not satisfy the bound Animal on parameter T, int given
Type argument 1 to call Service::spick() does not satisfy the bound Animal on parameter T, int given
