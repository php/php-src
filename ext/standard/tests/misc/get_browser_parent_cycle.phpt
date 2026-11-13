--TEST--
get_browser() handles valid parent chains and parent cycles
--INI--
browscap={PWD}/browscap_parent_cycle.ini
--FILE--
<?php

$normal = get_browser('normalchild', true);
echo "normal child: ";
var_dump($normal['normal_child']);
echo "normal parent: ";
var_dump($normal['normal_parent']);
echo "normal grandparent: ";
var_dump($normal['normal_grandparent']);
echo "normal shared: ";
var_dump($normal['shared']);

$cycle = get_browser('cyclea', true);
echo "cycle a: ";
var_dump($cycle['cycle_a']);
echo "cycle b: ";
var_dump($cycle['cycle_b']);
echo "cycle c: ";
var_dump($cycle['cycle_c']);
echo "cycle shared: ";
var_dump($cycle['shared']);

$entry = get_browser('cycleentry', true);
echo "cycle entered from outside: ";
var_dump($entry['cycle_entry'], $entry['cycle_a'], $entry['cycle_c'], $entry['shared']);
?>
--EXPECT--
normal child: string(5) "child"
normal parent: string(6) "parent"
normal grandparent: string(11) "grandparent"
normal shared: string(6) "parent"
cycle a: string(1) "a"
cycle b: string(1) "b"
cycle c: string(1) "c"
cycle shared: string(6) "cyclea"
cycle entered from outside: string(5) "entry"
string(1) "a"
string(1) "c"
string(6) "cyclea"
