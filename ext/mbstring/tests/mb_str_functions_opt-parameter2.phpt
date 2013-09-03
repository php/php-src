--TEST--
Optional long parameter might be null
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
echo mb_strpos('abb', 'b', default, 'UTF-8') . "\n";
echo mb_strrpos('abb', 'b', default, 'UTF-8') . "\n";
echo mb_stripos('abb', 'B', default, 'UTF-8') . "\n";
echo mb_strripos('abb', 'B', default, 'UTF-8') . "\n";
echo mb_strstr('foobarbaz', 'ba', default, 'UTF-8') . "\n";
echo mb_strrchr('foobarbaz', 'ba', default, 'UTF-8') . "\n";
echo mb_stristr('foobarbaz', 'BA', default, 'UTF-8') . "\n";
echo mb_strrichr('foobarbaz', 'BA', default, 'UTF-8') . "\n";
echo mb_substr('foobarbaz', 6, default, 'UTF-8') . "\n";
echo mb_strcut('foobarbaz', 6, default, 'UTF-8') . "\n";
echo mb_strimwidth('foobar', 0, 3, default, 'UTF-8') . "\n";
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
