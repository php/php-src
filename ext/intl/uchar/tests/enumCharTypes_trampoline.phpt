--TEST--
IntlChar::enumCharTypes() with trampoline callbacks
--EXTENSIONS--
intl
--FILE--
<?php
function testTrampoline(string $name, array $arguments): bool {
    // Only print the first two ranges to avoid ICU version dependencies.
    if ($arguments[0] < 33) {
        echo $name, ': ', implode(', ', $arguments), "\n";
    }
    if ($name === 'trampolineThrow' && $arguments[0] === 32) {
        throw new Exception('Stop enumeration');
    }
    return false;
}

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        return testTrampoline($name, $arguments);
    }
    public static function __callStatic(string $name, array $arguments) {
        return testTrampoline($name, $arguments);
    }
}

foreach ([new TrampolineTest(), TrampolineTest::class] as $target) {
    echo is_object($target) ? "__call\n" : "__callStatic\n";
    foreach (['trampoline', 'trampolineThrow', 'trampoline'] as $method) {
        try {
            var_dump(IntlChar::enumCharTypes([$target, $method]));
        } catch (Exception $e) {
            echo $e->getMessage(), "\n";
        }
    }
}
?>
--EXPECT--
__call
trampoline: 0, 32, 15
trampoline: 32, 33, 12
NULL
trampolineThrow: 0, 32, 15
trampolineThrow: 32, 33, 12
Stop enumeration
trampoline: 0, 32, 15
trampoline: 32, 33, 12
NULL
__callStatic
trampoline: 0, 32, 15
trampoline: 32, 33, 12
NULL
trampolineThrow: 0, 32, 15
trampolineThrow: 32, 33, 12
Stop enumeration
trampoline: 0, 32, 15
trampoline: 32, 33, 12
NULL
