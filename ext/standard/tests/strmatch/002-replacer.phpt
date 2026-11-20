--TEST--
Str\Replacer: replace with strtr semantics (longest-match, non-overlapping)
--FILE--
<?php
$r = new Str\Replacer(['cat' => 'dog', 'cats' => 'DOGS', 'a' => 'A']);
var_dump($r->replace('a cat and cats'));
var_dump($r->replace('nothing here'));
?>
--EXPECT--
string(14) "A dog And DOGS"
string(12) "nothing here"
