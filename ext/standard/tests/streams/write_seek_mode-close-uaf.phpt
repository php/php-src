--TEST--
`write_seek_mode` with a stringable object closing the stream triggers UAF
--EXTENSIONS--
zlib
--CREDITS--
Calif.io
--FILE--
<?php

$stream = fopen('php://temp', 'w+');

final class CloseStreamForWriteSeekMode
{
    public function __toString(): string
    {
        fclose($GLOBALS['stream']);
        return 'reset';
    }
}

$filter = stream_filter_append(
    $stream,
    'zlib.deflate',
    STREAM_FILTER_WRITE,
    ['write_seek_mode' => new CloseStreamForWriteSeekMode()],
);

var_dump(is_resource($filter), is_resource($stream));
?>
--EXPECTF--
Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d
bool(true)
bool(true)
