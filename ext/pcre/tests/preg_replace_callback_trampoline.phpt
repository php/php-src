--TEST--
preg_replace_callback() with a trampoline
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
        '@\b\w{1,2}\b@',
        '~\A.~',
    ],
    '@\b\w{1,2}\b@',
    [
        new stdClass(),
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

foreach ([$callback, $callbackThrow] as $fn) {
    foreach ($regexesToTest as $regex) {
        foreach ($subjectsToTest as $subject) {
            try {
                $matches = preg_replace_callback($regex, $fn, $subject);
                var_dump($matches);
            } catch (Throwable $e) {
                echo $e::class, ': ', $e->getMessage(), PHP_EOL;
            }
        }
    }
}

?>
--EXPECTF--
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline

Warning: Array to string conversion in %s on line %d
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
TypeError: preg_replace_callback(): Argument #3 ($subject) must be of type array|string, stdClass given
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline
Trampoline for trampoline

Warning: Array to string conversion in %s on line %d
array(4) {
  [0]=>
  string(12) "'a' 'b3' bcd"
  ["v"]=>
  string(6) "aksfjk"
  [12]=>
  string(9) "'aa' 'bb'"
  [13]=>
  string(5) "Array"
}
Trampoline for trampoline
Trampoline for trampoline
string(12) "'a' 'b3' bcd"
Error: Object of class stdClass could not be converted to string
TypeError: preg_replace_callback(): Argument #3 ($subject) must be of type array|string, stdClass given

Warning: Array to string conversion in %s on line %d
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
TypeError: preg_replace_callback(): Argument #3 ($subject) must be of type array|string, stdClass given
Trampoline for trampolineThrow

Warning: Array to string conversion in %s on line %d
Exception: boo
Trampoline for trampolineThrow
Exception: boo
Error: Object of class stdClass could not be converted to string
TypeError: preg_replace_callback(): Argument #3 ($subject) must be of type array|string, stdClass given
Trampoline for trampolineThrow

Warning: Array to string conversion in %s on line %d
Exception: boo
Trampoline for trampolineThrow
Exception: boo
Error: Object of class stdClass could not be converted to string
TypeError: preg_replace_callback(): Argument #3 ($subject) must be of type array|string, stdClass given

Warning: Array to string conversion in %s on line %d
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
TypeError: preg_replace_callback(): Argument #3 ($subject) must be of type array|string, stdClass given
