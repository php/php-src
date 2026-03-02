--TEST--
Bug #77950 (Heap-buffer-overflow in _estrndup via exif_process_IFD_TAG)
--EXTENSIONS--
exif
--FILE--
<?php
exif_read_data(__DIR__."/bug77950.tiff");
?>
DONE
--EXPECTF--
%A
DONE