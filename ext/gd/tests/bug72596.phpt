--TEST--
Bug #72596 (imagetypes function won't advertise WEBP support)
--EXTENSIONS--
gd
--FILE--
<?php
var_dump(function_exists('imagewebp') === (bool) (imagetypes() & IMG_WEBP));
?>
--EXPECT--
bool(true)
