--TEST--
SPL: RegexIterator
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

$ar = array(0, "123", 123, 22 => "abc", "a2b", 22, "a2d" => 7, 42);

foreach(new RegexIterator(new ArrayIterator($ar), "/2/") as $k => $v)
{
	echo "$k=>$v\n";
}

?>
===KEY===
<?php

foreach(new RegexIterator(new ArrayIterator($ar), "/2/", 0, RegexIterator::USE_KEY) as $k => $v)
{
	echo "$k=>$v\n";
}

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
1=>123
2=>123
23=>a2b
24=>22
25=>42
===KEY===
2=>123
22=>abc
23=>a2b
24=>22
a2d=>7
25=>42
===DONE===
