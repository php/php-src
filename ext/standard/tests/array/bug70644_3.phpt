--TEST--
Bug #70644: trivial hash complexity DoS attack (parse_str)
--INI--
max_input_vars=10000000
--FILE--
<?php

$s = 1 << 16;
$a = [];
for ($i = 0, $j = 0; $i < $s; $i++, $j += $s) {
    $a[] = 'a[' . $j  . ']=1';
}
parse_str(implode('&', $a), $arr);
var_dump(count($arr) < $s); // verify that parsing had properly been aborted upon collision maximum

?>
--EXPECT--
bool(true)
