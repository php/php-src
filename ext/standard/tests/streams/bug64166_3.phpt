--TEST--
Bug #64166: quoted-printable-encode stream filter incorrectly discarding whitespace - writes
--FILE--
<?php

function test_64166($data) {
    $fd = fopen('php://temp', 'w+');
    $res = stream_filter_append($fd, 'convert.quoted-printable-encode', STREAM_FILTER_WRITE, array(
		'line-break-chars' => "\n",
		'line-length' => 74
    ));
    fwrite($fd, $data);
    rewind($fd);

    var_dump(stream_get_contents($fd, -1, 0));

    stream_filter_remove($res);

    rewind($fd);
    stream_filter_append($fd, 'convert.quoted-printable-encode', STREAM_FILTER_WRITE, array(
		'line-break-chars' => "\n",
		'line-length' => 6
    ));
    fwrite($fd, $data);
    rewind($fd);
    var_dump(stream_get_contents($fd, -1, 0));

    fclose($fd);
}

test_64166("FIRST \nSECOND");
test_64166("FIRST  \nSECOND");

?>
--EXPECT--
string(15) "FIRST=20
SECOND"
string(19) "FIRST=
=20
SECON=
D"
string(18) "FIRST=20=20
SECOND"
string(24) "FIRST=
=20=
=20
SECON=
D"
