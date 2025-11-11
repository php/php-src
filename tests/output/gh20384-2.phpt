--TEST--
GH-20384: Ensure ob_gzhandler() is not affected by deprecation of ob_start() callback signatures - variation 2
--EXTENSIONS--
zlib
--FILE--
<?php
ob_start(ob_gzhandler(...));

echo '';
?>
DONE
--EXPECT--
DONE
