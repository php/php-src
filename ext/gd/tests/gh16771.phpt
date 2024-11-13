--TEST--
GH-16771 (UBSan abort in ext/gd/libgd/gd.c:1372)
--EXTENSIONS--
gd
--FILE--
<?php
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OIMzTvvJXvvJbml6XmnKzoqp7jg4bjgq3jgrnjg4g=');
imagecreatefromstring($string_mb);
--EXPECTF--
Warning: imagecreatefromstring(): Data is not in a recognized format in %s on line %d
