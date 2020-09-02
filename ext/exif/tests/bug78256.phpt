--TEST--
Bug #78256 (heap-buffer-overflow on exif_process_user_comment)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
@exif_read_data(__DIR__."/bug78256.jpg", 'COMMENT', FALSE, TRUE);
?>
DONE
--EXPECT--
DONE
