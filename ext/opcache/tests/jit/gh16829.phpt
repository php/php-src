--TEST--
GH-16829 (Segmentation fault with opcache.jit=tracing enabled on aarch64)
--INI--
opcache.jit_buffer_size=32M
--EXTENSIONS--
opcache
--FILE--
<?php
touch(__DIR__ . '/gh16829_1.inc');
require_once(__DIR__ . '/gh16829_1.inc');
?>
DONE
--EXPECT--
DONE
