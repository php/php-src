--TEST--
Optional long parameter might be null
--FILE--
<?php
echo mb_strpos('abb', 'b', null, 'UTF-8') . "\n";
echo mb_strrpos('abb', 'b', null, 'UTF-8') . "\n";
echo mb_stripos('abb', 'B', null, 'UTF-8') . "\n";
echo mb_strripos('abb', 'B', null, 'UTF-8') . "\n";
echo mb_strstr('foobarbaz', 'ba', null, 'UTF-8') . "\n";
echo mb_strrchr('foobarbaz', 'ba', null, 'UTF-8') . "\n";
echo mb_stristr('foobarbaz', 'BA', null, 'UTF-8') . "\n";
echo mb_strrichr('foobarbaz', 'BA', null, 'UTF-8') . "\n";
echo mb_substr('foobarbaz', 6, null, 'UTF-8') . "\n";
echo mb_strcut('foobarbaz', 6, null, 'UTF-8') . "\n";
echo mb_strimwidth('foobar', 0, 3, null, 'UTF-8') . "\n";
?>
==DONE==
--EXPECT--
1
2
1
2
barbaz
baz
barbaz
baz
baz
baz
foo
==DONE==
