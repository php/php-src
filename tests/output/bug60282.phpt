--TEST--
Bug #60282 (Segfault when using ob_gzhandler() with open buffers)
--EXTENSIONS--
zlib
--FILE--
<?php
ob_start();
ob_start();
ob_start('ob_gzhandler');
echo "done";
?>
--EXPECT--
done
