--TEST--
IDN support
--INI--
allow_url_fopen=1
--FILE--
<?php
$fd = fopen('http://académie-française.fr', 'rb');
$meta = stream_get_meta_data($fd);
var_dump($meta['wrapper_data'][0]);
fclose($fd);

--EXPECT--
string(15) "HTTP/1.1 200 OK"
