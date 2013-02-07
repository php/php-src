--TEST--
Bug #64166: quoted-printable-encode stream filter incorrectly discarding whitespace
--FILE--
<?php
$data = "FIRST \nSECOND";

$fd = fopen('php://temp', 'w+');
fwrite($fd, $data);
rewind($fd);

$res = stream_filter_append($fd, 'convert.quoted-printable-encode', STREAM_FILTER_READ, array(
	'line-break-chars' => "\n",
	'line-length' => 7
));
var_dump(stream_get_contents($fd, -1, 0));

stream_filter_remove($res);

rewind($fd);
stream_filter_append($fd, 'convert.quoted-printable-encode', STREAM_FILTER_READ, array(
	'line-break-chars' => "\n",
	'line-length' => 6
));
var_dump(stream_get_contents($fd, -1, 0));
?>
--EXPECT--
string(14) "FIRST =
SECOND"
string(18) "FIRST=
 =
SECON=
D"
