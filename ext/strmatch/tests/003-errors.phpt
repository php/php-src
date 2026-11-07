--TEST--
Str constructors reject empty keywords and non-string dictionary values
--EXTENSIONS--
strmatch
--FILE--
<?php
foreach ([
    [Str\Matcher::class, ['ok', '']],
    [Str\Matcher::class, ['ok', 123]],
    [Str\Replacer::class, ['ok' => 'value', '' => 'empty']],
    [Str\Replacer::class, ['ok' => 'value', 'bad' => 123]],
] as [$class, $dictionary]) {
    try {
        new $class($dictionary);
        echo "FAIL\n";
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
ValueError: Str\Matcher::__construct(): Argument #1 ($keywords) must not contain an empty keyword
TypeError: Str\Matcher::__construct(): Argument #1 ($keywords) must be an array of strings
ValueError: Str\Replacer::__construct(): Argument #1 ($replacements) must not contain an empty keyword
TypeError: Str\Replacer::__construct(): Argument #1 ($replacements) must be an array of string replacements
