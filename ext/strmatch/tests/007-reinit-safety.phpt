--TEST--
Str\Matcher and Str\Replacer reject repeated initialization, including empty dictionaries
--EXTENSIONS--
strmatch
--FILE--
<?php
foreach ([new Str\Matcher(['foo']), new Str\Matcher([]), new Str\Replacer(['x' => 'y']), new Str\Replacer([])] as $object) {
    try {
        $object->__construct([]);
    } catch (Error $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
$matcher = new Str\Matcher(['foo']);
try {
    $matcher->__construct(['bar']);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($matcher->matches('foo'), $matcher->matches('bar'));
$replacer = new Str\Replacer(['x' => 'y']);
try {
    $replacer->__construct(['x' => 'z']);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
echo $replacer->replace('x'), "\n";
?>
--EXPECT--
Error: Str\Matcher is already initialized
Error: Str\Matcher is already initialized
Error: Str\Replacer is already initialized
Error: Str\Replacer is already initialized
Error: Str\Matcher is already initialized
bool(true)
bool(false)
Error: Str\Replacer is already initialized
y
