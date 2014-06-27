--TEST--
basename bug #66395 check drive traversing and NTFS streams
--SKIPIF--
<?php if (substr(PHP_OS, 0, 3) != 'WIN') { die('skip Windows only basename tests'); } ?>
--FILE--
<?php
echo basename("y:") . "\n";
echo basename("y:/") . "\n";
echo basename("notdriveletter:file.txt") . "\n";
echo basename("a:\\b:c:d:hello.txt\\hcd:c.txt") . "\n";
echo basename("a:b:c:d:hello.txt\\d:some.txt") . "\n";
echo basename("a:b:c:d:hello\world\a.bmp\c:d:e:f.txt") . "\n";
echo basename("a:\\b:\\c:d:hello\\world\\a.bmp\\d:e:f:g.txt") . "\n";
echo basename("a:\\b:\\c:d:hello/world\\a.bmp\\d:\\e:\\f:g.txt") . "\n";
echo basename("a:\\b:/c:d:hello\\world:somestream") . "\n";
echo basename("a:\\b:\\c:d:hello\\world:some.stream") . "\n";
echo basename("a:/b:\\c:d:hello\\world:some.stream:\$DATA") . "\n";
echo basename("x:y:z:hello\world:my.stream:\$DATA") . "\n";
echo basename("a:\\b:\\c:d:hello\\world:c:\$DATA") . "\n";
echo basename("a:\\b:\\c:d:hello\\d:world:c:\$DATA") . "\n";
?>
==DONE==
--EXPECTF--
y
y
notdriveletter:file.txt
hcd:c.txt
some.txt
f.txt
g.txt
g.txt
world:somestream
world:some.stream
world:some.stream:$DATA
world:my.stream:$DATA
world:c:$DATA
world:c:$DATA
==DONE==
