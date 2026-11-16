--TEST--
Hash: hash_update_stream() test
--FILE--
<?php
$fp = tmpfile();
fwrite($fp, 'The quick brown fox jumped over the lazy dog.');
rewind($fp);

$ctx = hash_init('md5');
hash_update_stream($ctx, $fp);
echo "Single read: ", hash_final($ctx), "\n";
rewind($fp);

$ctx = hash_init('md5');
hash_update_stream($ctx, $fp, 20);
hash_update_stream($ctx, $fp);
echo "Chunked read: ", hash_final($ctx), "\n";
rewind($fp);

$ctx = hash_init('md5');
hash_update_stream($ctx, $fp, 0);
echo "Empty read: ", hash_final($ctx), "\n";
fclose($fp);
?>
--EXPECT--
Single read: 5c6ffbdd40d9556b73a21e63c3e0e904
Chunked read: 5c6ffbdd40d9556b73a21e63c3e0e904
Empty read: d41d8cd98f00b204e9800998ecf8427e
