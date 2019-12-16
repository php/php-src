--TEST--
Bug #78793: Use-after-free in exif parsing under memory sanitizer
--FILE--
<?php
$f = "ext/exif/tests/bug77950.tiff";
for ($i = 0; $i < 10; $i++) {
    @exif_read_data($f);
}
?>
===DONE===
--EXPECT--
===DONE===
