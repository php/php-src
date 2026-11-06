--TEST--
in_array()/array_search() strict mode with a string needle
--FILE--
<?php

function dyn(string $s): string { return $s . ''; }

echo "-- found / not found --\n";
$list = ['alpha', 'beta', '', "a\0b", 'kappa'];
var_dump(in_array('kappa', $list, true));
var_dump(in_array('nope', $list, true));
var_dump(in_array('', $list, true));
var_dump(in_array("a\0b", $list, true));
var_dump(in_array("a\0c", $list, true));
var_dump(in_array('alph', $list, true));
var_dump(array_search('beta', $list, true));
var_dump(array_search('nope', $list, true));

echo "-- non-interned strings --\n";
var_dump(in_array(dyn('beta'), $list, true));
var_dump(in_array('beta', [dyn('beta')], true));
var_dump(array_search(dyn('kappa'), [dyn('kappa')], true));

echo "-- string and integer keys --\n";
var_dump(array_search('b', ['x' => 'a', 'y' => 'b', 7 => 'c'], true));
var_dump(array_search('c', ['x' => 'a', 'y' => 'b', 7 => 'c'], true));

echo "-- elements of other types never match --\n";
var_dump(in_array('1', [1, 1.0, true, [1], null], true));
var_dump(in_array('', [null, false, 0, []], true));
var_dump(in_array('abc', [0, true, new stdClass], true));
var_dump(array_search('1', [1, '1'], true));

echo "-- references --\n";
$s = 'ref';
$arr = [1, &$s, 3];
var_dump(array_search('ref', $arr, true));
$s = 'changed';
var_dump(in_array('ref', $arr, true));
var_dump(array_search('changed', $arr, true));
$i = 5;
var_dump(in_array('5', ['a', &$i], true));

echo "-- holes --\n";
$packed = ['a', 'b', 'c'];
unset($packed[1]);
var_dump(in_array('b', $packed, true));
var_dump(array_search('c', $packed, true));
$hash = ['x' => 'a', 'y' => 'b'];
unset($hash['x']);
var_dump(array_search('b', $hash, true));

echo "-- empty array --\n";
var_dump(in_array('a', [], true));

?>
--EXPECT--
-- found / not found --
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
int(1)
bool(false)
-- non-interned strings --
bool(true)
bool(true)
int(0)
-- string and integer keys --
string(1) "y"
int(7)
-- elements of other types never match --
bool(false)
bool(false)
bool(false)
int(1)
-- references --
int(1)
bool(false)
int(1)
bool(false)
-- holes --
bool(false)
int(2)
string(1) "y"
-- empty array --
bool(false)
