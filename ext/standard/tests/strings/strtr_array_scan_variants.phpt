--TEST--
strtr() array mode across single-byte, SIMD, bitset, and long-key scans
--FILE--
<?php
var_dump(strtr('abcdefghijklmnopqrstuvwxyz0123456789 {a} tail', ['{a}' => 'X', '{b}' => 'Y']));
var_dump(strtr('0123456789abcdef<tag>&"q">0123456789abcdef', ['<' => '[', '>' => ']', '&' => '+', '"' => "'"]));
var_dump(strtr('alpha beta gamma delta epsilon zeta', [
    'alpha' => '1',
    'beta' => '2',
    'gamma' => '3',
    'delta' => '4',
    'epsilon' => '5',
    'zeta' => '6',
]));
$longKey = str_repeat('K', 300);
var_dump(strtr("pre-{$longKey}-post", [$longKey => 'LONG', 'pre' => 'P']));
var_dump(strtr('id=12345 and 0', [12345 => 'N', 0 => 'Z', '' => 'ignored']));
var_dump(strtr(str_repeat('x', 64), ['{a}' => '1', '{b}' => '2']) === str_repeat('x', 64));
foreach ([255, 256, 257] as $n) {
    $k = str_repeat('k', $n);
    var_dump(strtr("<{$k}>[{$k}k]", [$k => 'L', $k . 'k' => 'M', '<' => '(']));
}
?>
--EXPECTF--
string(43) "abcdefghijklmnopqrstuvwxyz0123456789 X tail"
string(42) "0123456789abcdef[tag]+'q']0123456789abcdef"
string(11) "1 2 3 4 5 6"
string(11) "P-LONG-post"

Warning: strtr(): Ignoring replacement of empty string in %s on line %d
string(10) "id=N and Z"
bool(true)
string(6) "(L>[M]"
string(6) "(L>[M]"
string(6) "(L>[M]"
