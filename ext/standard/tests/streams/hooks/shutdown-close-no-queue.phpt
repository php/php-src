--TEST--
IO hooks: a stream closed after the request shutdown waits without recreating the core queue
--EXTENSIONS--
zlib
--FILE--
<?php
[$a, $b] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

/* Fill the socket buffers so the flush at close has to wait */
stream_set_blocking($a, false);
while (fwrite($a, str_repeat('x', 65536)) > 0);
stream_set_blocking($a, true);
stream_set_timeout($a, 0, 100000);
stream_filter_append($a, 'zlib.deflate', STREAM_FILTER_WRITE);
fwrite($a, random_bytes(100));
echo "end\n";
?>
--EXPECTF--
end

%s: PHP Request Shutdown: Send of %d bytes failed with errno=%d %s in Unknown on line 0
