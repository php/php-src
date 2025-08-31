--TEST--
Bug #78256 (heap-buffer-overflow on exif_process_user_comment)
--EXTENSIONS--
exif
--FILE--
<?php
@exif_read_data(__DIR__."/bug78256.jpg", 'COMMENT', FALSE, TRUE);
?>
DONE
--EXPECT--
DONE
