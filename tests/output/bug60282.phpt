--TEST--
Bug #60282 (Segfault when using ob_gzhandler() with open buffers)
--FILE--
<?php
ob_start();
ob_start();
ob_start('ob_gzhandler');
echo "done";
--EXPECT--
done
