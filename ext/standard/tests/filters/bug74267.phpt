--TEST--
Bug #74267 (segfault with streams and invalid data)
--FILE--
<?php
$stream = fopen('php://memory', 'w');
stream_filter_append($stream, 'convert.quoted-printable-decode', STREAM_FILTER_WRITE, ['line-break-chars' => "\r\n"]);

$lines = [
    "\r\n",
    " -=()\r\n",
    " -=\r\n",
    "\r\n"
    ];

foreach ($lines as $line) {
    fwrite($stream, $line);
}

fclose($stream);
echo "done\n";
?>
--EXPECTF--
Warning: fwrite(): Stream filter (convert.quoted-printable-decode): invalid byte sequence in %s on line %d

Warning: fwrite(): Stream filter (convert.quoted-printable-decode): invalid byte sequence in %s on line %d
done
