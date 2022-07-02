--TEST--
Union with anonymous class type
--FILE--
<?php

$a = new class {
    public function testParam(self|string $a)
    {
    }
    public function test(): self|string
    {
        return new \stdClass;
    }
};

try {
    $a->testParam(null);
} catch (\Throwable $e) {
    echo $e->getMessage()."\n";
}

try {
    $a->test();
} catch (\Throwable $e) {
    echo $e->getMessage()."\n";
}
?>
--EXPECTF--
class@anonymous(): Argument #1 ($a) must be of type class@anonymous|string, null given, called in %s on line %d
class@anonymous::test(): Return value must be of type class@anonymous|string, stdClass returned
