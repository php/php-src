--TEST--
convert.* write filter: write_seek_mode parameter
--FILE--
<?php
/* Smoke test: the write_seek_mode parameter is accepted on convert.* filters
 * and behaves correctly per mode. The deeper reset semantics are exercised
 * via the read path for convert.* (read_seekable = START always resets) and
 * via the dedicated zlib/bz2 mode tests. */

foreach (['convert.base64-encode', 'convert.quoted-printable-encode'] as $name) {
    /* preserve: seeks succeed (default) */
    $fp = fopen('php://memory', 'w+');
    stream_filter_append($fp, $name, STREAM_FILTER_WRITE,
        ['write_seek_mode' => 'preserve']);
    fwrite($fp, 'Hello');
    var_dump(fseek($fp, 0, SEEK_SET) === 0);
    var_dump(fseek($fp, 100, SEEK_SET) === 0);
    fclose($fp);

    /* reset: seeks succeed, callback dispatched */
    $fp = fopen('php://memory', 'w+');
    stream_filter_append($fp, $name, STREAM_FILTER_WRITE,
        ['write_seek_mode' => 'reset']);
    fwrite($fp, 'Hello');
    var_dump(fseek($fp, 0, SEEK_SET) === 0);
    fclose($fp);

    /* strict: seek fails */
    $fp = fopen('php://memory', 'w+');
    stream_filter_append($fp, $name, STREAM_FILTER_WRITE,
        ['write_seek_mode' => 'strict']);
    fwrite($fp, 'Hello');
    var_dump(@fseek($fp, 0, SEEK_SET) === -1);
    fclose($fp);

    /* invalid: ValueError */
    $fp = fopen('php://memory', 'w+');
        stream_filter_append($fp, $name, STREAM_FILTER_WRITE,
            ['write_seek_mode' => 42]);
    if ($fp) {
        fclose($fp);
    }
}
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)

Warning: stream_filter_append(): "write_seek_mode" filter parameter must be one of "preserve", "reset", or "strict" in %s

Warning: stream_filter_append(): Unable to create or locate filter "convert.base64-encode" in %s
bool(true)
bool(true)
bool(true)
bool(true)

Warning: stream_filter_append(): "write_seek_mode" filter parameter must be one of "preserve", "reset", or "strict" in %s

Warning: stream_filter_append(): Unable to create or locate filter "convert.quoted-printable-encode" in %s
