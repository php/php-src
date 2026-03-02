--TEST--
Hash: hash_update_stream() test
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br
--FILE--
<?php
$fp = tmpfile();
fwrite($fp, 'The quick brown fox jumped over the lazy dog.');
rewind($fp);

$ctx = hash_init('md5');
hash_update_stream($ctx, $fp);
echo hash_final($ctx);
?>
--EXPECT--
5c6ffbdd40d9556b73a21e63c3e0e904
