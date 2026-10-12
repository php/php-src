--TEST--
preg_replace_callback() with a trampoline and a ZPP failure
--FILE--
<?php

class TrampolineTest {
    public function __call(string $name, array $arguments): string {
        echo 'Trampoline for ', $name, PHP_EOL;
        if ($name === 'trampolineThrow') {
            throw new Exception('boo');
        }
        return "'" . $arguments[0][0] . "'";
    }
}
$o = new TrampolineTest();
$callback = [$o, 'trampoline'];
$callbackThrow = [$o, 'trampolineThrow'];

$pattern = '@\b\w{1,2}\b@';

$subject = 'a b3 bcd';

$badLimit = 'not an int';

try {
    $matches = preg_replace_callback($pattern, $callbackThrow, $subject, $badLimit);
    var_dump($matches);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: preg_replace_callback(): Argument #4 ($limit) must be of type int, string given
