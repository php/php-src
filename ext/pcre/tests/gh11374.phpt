--TEST--
GH-11374 (PCRE regular expression without JIT enabled gives different result)
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (!zend_test_is_pcre_bundled() && (PCRE_VERSION_MAJOR == 10 && PCRE_VERSION_MINOR <= 42)) die("skip old pcre version");
?>
--FILE--
<?php

$regex = '
    (?<types>
        (?:
            (?:\{ (?&types) \})
            | (a)
        )
        (\*?)
    )
';

ini_set('pcre.jit', '0');
$res = preg_match('{^' . $regex . '$}x', '{a}', $matches, PREG_OFFSET_CAPTURE);
ini_set('pcre.jit', '1');
// regex must be different to prevent regex cache, so just add 2nd "x" modifier
$res2 = preg_match('{^' . $regex . '$}xx', '{a}', $matches2, PREG_OFFSET_CAPTURE);

var_dump($matches === $matches2);
print_r($matches);

?>
--EXPECT--
bool(true)
Array
(
    [0] => Array
        (
            [0] => {a}
            [1] => 0
        )

    [types] => Array
        (
            [0] => {a}
            [1] => 0
        )

    [1] => Array
        (
            [0] => {a}
            [1] => 0
        )

    [2] => Array
        (
            [0] => 
            [1] => -1
        )

    [3] => Array
        (
            [0] => 
            [1] => 3
        )

)
