--TEST--
Implicit break in match expression
--FILE--
<?php

function dump_and_return($string) {
    var_dump($string);
    return $string;
}

var_dump(match ('foo') {
    'foo' => dump_and_return('foo'),
    'bar' => dump_and_return('bar'),
});

?>
--EXPECT--
string(3) "foo"
string(3) "foo"
