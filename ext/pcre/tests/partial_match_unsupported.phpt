--TEST--
Partial matching is only supported by preg_match() and preg_match_all()
--FILE--
<?php

$regex = '/abc/p';
$subject = 'foobar ab';

var_dump(preg_replace($regex, 'replacement', $subject));
var_dump(preg_replace_callback($regex, function() {}, $subject));
var_dump(preg_replace_callback_array([$regex => function() {}], $subject));

var_dump(preg_grep($regex, [$subject]));
var_dump(preg_filter($regex, 'replacement', [$subject]));
var_dump(preg_split($regex, $subject));
?>
--EXPECTF--
Warning: preg_replace(): Partial matching is not supported in %s on line %d
NULL

Warning: preg_replace_callback(): Partial matching is not supported in %s on line %d
NULL

Warning: preg_replace_callback_array(): Partial matching is not supported in %s on line %d
NULL

Warning: preg_grep(): Partial matching is not supported in %s on line %d
NULL

Warning: preg_filter(): Partial matching is not supported in %s on line %d
array(0) {
}

Warning: preg_split(): Partial matching is not supported in %s on line %d
bool(false)
