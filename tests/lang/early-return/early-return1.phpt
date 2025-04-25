--TEST--
Memory leaks
--FILE--
<?php
function test($input = null, $fallback = 'fallback') {
    $return = $input ?: return $fallback;
    var_dump($return);
}
function test2($input = null, $fallback = 'fallback') {
    $return = $input ?? return $fallback;
    var_dump($return);
}

echo test('hello');
echo test(false, 'false');
echo test(null, 'null');

echo PHP_EOL;

echo test2('world');
echo test2(false, 'false');
echo test2(null, 'null');

?>
--EXPECT--
string(5) "hello"
falsenull
string(5) "world"
bool(false)
null