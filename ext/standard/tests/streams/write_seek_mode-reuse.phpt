--TEST--
`write_seek_mode` with a stringable object closing and reopening the stream triggers reuse
--EXTENSIONS--
zlib
--CREDITS--
Calif.io
--FILE--
<?php

$stream = fopen('php://temp', 'w+');
$replacementStream = null;

final class ReplaceStreamForWriteSeekMode
{
    public function __toString(): string
    {
        fclose($GLOBALS['stream']);
        $GLOBALS['replacementStream'] = fopen('php://temp', 'w+');
        return 'reset';
    }
}

$filter = stream_filter_append(
    $stream,
    'zlib.deflate',
    STREAM_FILTER_WRITE,
    ['write_seek_mode' => new ReplaceStreamForWriteSeekMode()],
);

var_dump(is_resource($filter), is_resource($replacementStream));
fwrite($replacementStream, str_repeat('A', 64));
var_dump(stream_filter_remove($filter));
rewind($replacementStream);
$raw = stream_get_contents($replacementStream);
var_dump(strlen($raw), bin2hex($raw));
?>
--EXPECTF--
Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d
bool(true)
bool(true)
bool(true)
int(64)
string(128) "41414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141414141"
