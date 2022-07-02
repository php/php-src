--TEST--
Incorrect DCE of FREE of COALESCE
--FILE--
<?php

function test(?string $str) {
    $str ?? $str = '';
    return strlen($str);
}

$foo = 'foo';
$foo .= 'bar';
var_dump(test($foo));

?>
--EXPECT--
int(6)
