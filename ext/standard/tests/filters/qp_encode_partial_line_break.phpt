--TEST--
convert.quoted-printable-encode keeps a partial line break sequence until more input or the end of the stream
--FILE--
<?php
function encode(array $chunks): string {
    $fp = fopen('php://temp', 'w+');
    $filter = stream_filter_append($fp, 'convert.quoted-printable-encode', STREAM_FILTER_WRITE, [
        'line-length' => 76,
        'line-break-chars' => "\r\n",
    ]);
    foreach ($chunks as $chunk) {
        fwrite($fp, $chunk);
    }
    stream_filter_remove($filter);
    rewind($fp);
    return stream_get_contents($fp);
}

foreach ([["a\r"], ["a", "=\r"], ["Hello=\r", "\nWorld"], ["Hello=\r\n"], ["=\r\n\r", "\n"]] as $chunks) {
    $encoded = encode($chunks);
    echo json_encode($encoded), "\n";
    var_dump(quoted_printable_decode($encoded) === implode('', $chunks));
}
?>
--EXPECT--
"a=0D"
bool(true)
"a=3D=0D"
bool(true)
"Hello=3D\r\nWorld"
bool(true)
"Hello=3D\r\n"
bool(true)
"=3D\r\n\r\n"
bool(true)
