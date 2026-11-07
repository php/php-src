--TEST--
Str\Replacer: replace with strtr semantics (longest-match, non-overlapping) and count
--EXTENSIONS--
strmatch
--FILE--
<?php
$r = new Str\Replacer(['cat' => 'dog', 'cats' => 'DOGS', 'a' => 'A']);
$n = 0;
var_dump($r->replace('a cat and cats', $n));
var_dump($n);
var_dump($r->replace('nothing here'));
?>
--EXPECT--
string(14) "A dog And DOGS"
int(4)
string(12) "nothing here"
