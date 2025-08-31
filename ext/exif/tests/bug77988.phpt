--TEST--
Bug #77988 (heap-buffer-overflow on php_jpg_get16)
--EXTENSIONS--
exif
--FILE--
<?php
exif_read_data(__DIR__."/bug77988.jpg", 'COMMENT', FALSE, TRUE);
?>
DONE
--EXPECT--
DONE
