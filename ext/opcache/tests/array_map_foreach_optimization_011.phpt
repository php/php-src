--TEST--
array_map(): foreach optimization - pfa pre-bound arg reexecution bug
--CREDITS--
Ryan @ Calif.io
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php

function cand_86013_bound_value(): string
{
    global $bound_calls;
    echo 'BOUND:', ++$bound_calls, "\n";
    return 'b';
}

function cand_86013_input(): array
{
    echo "INPUT\n";
    return ['a', 'a', 'a'];
}

$bound_calls = 0;
echo "direct array_map\n";
$direct = array_map(
    str_replace('a', cand_86013_bound_value(), ?),
    cand_86013_input(),
);
var_dump($direct, $bound_calls);

$bound_calls = 0;
$array_map = 'array_map';
echo "dynamic-call control\n";
$control = $array_map(
    str_replace('a', cand_86013_bound_value(), ?),
    cand_86013_input(),
);
var_dump($control, $bound_calls);

?>
--EXPECT--
direct array_map
BOUND:1
INPUT
array(3) {
  [0]=>
  string(1) "b"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "b"
}
int(1)
dynamic-call control
BOUND:1
INPUT
array(3) {
  [0]=>
  string(1) "b"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "b"
}
int(1)
