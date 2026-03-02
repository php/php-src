--TEST--
Pipe operator rejects by-reference functions.
--FILE--
<?php

function _modify(int &$a): string {
    $a += 1;
    return "foo";
}

function _append(array &$a): string {
    $a['bar'] = 'beep';
}

// Simple variables
try {
    $a = 5;
    $res1 = $a |> _modify(...);
    var_dump($res1);
} catch (\Error $e) {
  echo $e->getMessage(), PHP_EOL;
}

// Complex variables.
try {
    $a = ['foo' => 'beep'];
    $res2 = $a |> _append(...);
    var_dump($res2);
} catch (\Error $e) {
  echo $e->getMessage(), PHP_EOL;
}


?>
--EXPECTF--
_modify(): Argument #1 ($a) could not be passed by reference
_append(): Argument #1 ($a) could not be passed by reference
