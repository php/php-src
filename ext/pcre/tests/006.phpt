--TEST--
preg_replace() with array of failing regular expressions
--INI--
pcre.jit=0
pcre.backtrack_limit=100000
--FILE--
<?php

$text = '[CODE]&lt;td align=&quot;$stylevar[right]&quot;&gt;[/CODE]';
$result = preg_replace(array('#\[(right)\](((?R)|[^[]+?|\[)*)\[/\\1\]#siU', '#\[(right)\](((?R)|[^[]+?|\[)*)\[/\\1\]#siU'), '', $text);
var_dump($text);
var_dump($result);

$result = preg_replace('#\[(right)\](((?R)|[^[]+?|\[)*)\[/\\1\]#siU', '', $text);
var_dump($text);
var_dump($result);

echo "Done\n";
?>
--EXPECTF--
string(58) "[CODE]&lt;td align=&quot;$stylevar[right]&quot;&gt;[/CODE]"
NULL
string(58) "[CODE]&lt;td align=&quot;$stylevar[right]&quot;&gt;[/CODE]"
NULL
Done
