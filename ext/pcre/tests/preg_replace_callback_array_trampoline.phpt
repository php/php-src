--TEST--
preg_replace_callback_array() with a trampoline
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

$regexesToTest = [
    [
        '@\b\w{1,2}\b@' => $callback,
        '~\A.~' => $callback,
    ],
    [
        '@\b\w{1,2}\b@' => $callback,
        '~\A.~' => $callbackThrow,
    ],
    [
        '@\b\w{1,2}\b@' => $callback,
        '~\A.~' => new stdClass(),
    ],
];

$subjectsToTest = [
    [
        'a b3 bcd',
        'v' => 'aksfjk',
        12 => 'aa bb',
        ['xyz'],
    ],
    'a b3 bcd',
    [
        new stdClass(),
    ],
    new stdClass(),
];

foreach ($regexesToTest as $regex) {
    foreach ($subjectsToTest as $subject) {
        try {
            $matches = preg_replace_callback_array($regex, $subject);
            var_dump($matches);
        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), PHP_EOL;
        }
    }
}

?>
--EXPECTF--
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline

Warning: Array to string conversion in %s on line %d
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline
array(4) {
  [0]=>
  string(14) "'''a' 'b3' bcd"
  ["v"]=>
  string(8) "'a'ksfjk"
  [12]=>
  string(11) "'''aa' 'bb'"
  [13]=>
  string(7) "'A'rray"
}
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline
string(14) "'''a' 'b3' bcd"
Error: Object of class stdClass could not be converted to string
TypeError: preg_replace_callback_array(): Argument #2 ($subject) must be of type array|string, stdClass given
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline

Warning: Array to string conversion in %s on line %d
Trampoline for trampolineThrow
Exception: boo
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampolineThrow
Exception: boo
Error: Object of class stdClass could not be converted to string
TypeError: preg_replace_callback_array(): Argument #2 ($subject) must be of type array|string, stdClass given
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline

Warning: Array to string conversion in %s on line %d
TypeError: preg_replace_callback_array(): Argument #1 ($pattern) must contain only valid callbacks
Trampoline for trampoline
Trampoline for trampoline
TypeError: preg_replace_callback_array(): Argument #1 ($pattern) must contain only valid callbacks
Error: Object of class stdClass could not be converted to string
TypeError: preg_replace_callback_array(): Argument #2 ($subject) must be of type array|string, stdClass given
