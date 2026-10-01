--TEST--
stream_copy_to_stream() with a partially read file-backed php://temp source
--FILE--
<?php

/* Spill php://temp to its tmpfile backing so the copy source delegates to an
 * inner stdio stream whose buffer state the fd-level fast path cannot see. */
$src = fopen('php://temp/maxmemory:16', 'r+');
fwrite($src, str_repeat("A", 2000) . str_repeat("B", 2000));
rewind($src);
/* Read-ahead on the inner stream moves its fd offset past the position. */
var_dump(strlen(fread($src, 2000)));

$dst = tmpfile();
$copied = stream_copy_to_stream($src, $dst);
var_dump($copied);

rewind($dst);
var_dump(stream_get_contents($dst) === str_repeat("B", 2000));

fclose($src);
fclose($dst);
?>
--EXPECT--
int(2000)
int(2000)
bool(true)
