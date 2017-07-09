--TEST--
Bug #63217 (Constant numeric strings become integers when used as ArrayAccess offset)
--INI--
opcache.enable_cli=1
opcache.enable=1
opcache.optimization_level=-1
--FILE--
<?php

class Test implements ArrayAccess {
    public function offsetExists($offset) {
        echo "offsetExists given ";
        var_dump($offset);
    }

    public function offsetUnset($offset) {
        echo "offsetUnset given ";
        var_dump($offset);
    }

    public function offsetSet($offset, $value) {
        echo "offsetSet given ";
        var_dump($offset);
    }

    public function offsetGet($offset) {
        echo "offsetGet given ";
        var_dump($offset);
    }
}

$test = new Test;

// These should all produce string(...) "..." output
// the incorrect output is int(...)
isset($test['0']);
isset($test['123']);
unset($test['0']);
unset($test['123']);
$test['0'] = true;
$test['123'] = true;
$foo = $test['0'];
$foo = $test['123'];

// These caused the same bug, but in opcache rather than the compiler
isset($test[(string)'0']);
isset($test[(string)'123']);
unset($test[(string)'0']);
unset($test[(string)'123']);
$test[(string)'0'] = true;
$test[(string)'123'] = true;
$foo = $test[(string)'0'];
$foo = $test[(string)'123'];

/**
 * @see https://github.com/php/php-src/pull/2607#issuecomment-313781748
 */
function test(): string {
    $array["10"] = 42;

    foreach ($array as $key => $value) {
        return $key;
    }
}

var_dump(test());

?>
--EXPECT--
offsetExists given string(1) "0"
offsetExists given string(3) "123"
offsetUnset given string(1) "0"
offsetUnset given string(3) "123"
offsetSet given string(1) "0"
offsetSet given string(3) "123"
offsetGet given string(1) "0"
offsetGet given string(3) "123"
offsetExists given string(1) "0"
offsetExists given string(3) "123"
offsetUnset given string(1) "0"
offsetUnset given string(3) "123"
offsetSet given string(1) "0"
offsetSet given string(3) "123"
offsetGet given string(1) "0"
offsetGet given string(3) "123"
string(2) "10"