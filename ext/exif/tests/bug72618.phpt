--TEST--
Bug 72618 (NULL Pointer Dereference in exif_process_user_comment)
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
var_dump(count(exif_read_data(dirname(__FILE__) . "/bug72618.jpg")));
?>
--EXPECTF--
Warning: exif_read_data(bug72618.jpg): IFD data bad offset: 0x058E length 0x0030 in %s/bug72618.php on line %d
int(13)