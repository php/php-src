--TEST--
implode() with an array first argument behaves the same in direct and dynamic calls
--FILE--
<?php
$array = ['a', 'b'];
$null = null;

function test(string $label, callable $direct, callable $dynamic) {
    foreach (['direct' => $direct, 'dynamic' => $dynamic] as $kind => $fn) {
        try {
            $result = var_export($fn(), true);
        } catch (TypeError $e) {
            $result = $e->getMessage();
        }
        echo "$label ($kind): $result\n";
    }
}

test('array, null', fn() => implode($array, $null), fn() => call_user_func('implode', $array, $null));
test('array, null literal', fn() => implode($array, null), fn() => call_user_func('implode', $array, null));
test('empty array, null', fn() => implode([], $null), fn() => call_user_func('implode', [], $null));
test('array, string', fn() => implode($array, ','), fn() => call_user_func('implode', $array, ','));
test('array, array', fn() => implode($array, $array), fn() => call_user_func('implode', $array, $array));
test('string, array', fn() => implode(',', $array), fn() => call_user_func('implode', ',', $array));
test('join', fn() => join($array, $null), fn() => call_user_func('join', $array, $null));
?>
--EXPECT--
array, null (direct): 'ab'
array, null (dynamic): 'ab'
array, null literal (direct): 'ab'
array, null literal (dynamic): 'ab'
empty array, null (direct): ''
empty array, null (dynamic): ''
array, string (direct): implode(): Argument #2 ($array) must be of type ?array, string given
array, string (dynamic): implode(): Argument #2 ($array) must be of type ?array, string given
array, array (direct): implode(): Argument #1 ($separator) must be of type string, array given
array, array (dynamic): implode(): Argument #1 ($separator) must be of type string, array given
string, array (direct): 'a,b'
string, array (dynamic): 'a,b'
join (direct): 'ab'
join (dynamic): 'ab'
