--TEST--
addcslashes() and stripcslashes() function
--FILE--
<?php
echo addcslashes(b"", b"")."\n";
echo addcslashes(b"", b"burp")."\n";
echo addcslashes(b"kaboemkara!", b"")."\n";
echo addcslashes(b"foobarbaz", b'bar')."\n";
echo addcslashes(b'foo[ ]', b'A..z')."\n";
echo @addcslashes(b"zoo['.']", b'z..A')."\n";
echo addcslashes(b'abcdefghijklmnopqrstuvwxyz', b"a\145..\160z")."\n";
echo "\n\r" == stripcslashes(b'\n\r'),"\n";
echo stripcslashes(b'\065\x64')."\n";
echo stripcslashes(b'')."\n";
?>
--EXPECT--


kaboemkara!
foo\b\a\r\b\az
\f\o\o\[ \]
\zoo['\.']
\abcd\e\f\g\h\i\j\k\l\m\n\o\pqrstuvwxy\z
1
5d
