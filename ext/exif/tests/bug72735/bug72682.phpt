--TEST--
Bug #72735 MakerNote regression
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
foreach (['nokia.jpg', 'samsung.jpg', 'panasonic.jpg'] as $picture) {
    echo $picture . ': ';

    $info = exif_read_data(__DIR__ . DIRECTORY_SEPARATOR . $picture);
    var_dump($info['MakerNote']);
}
?>
--EXPECT--
nokia.jpg: string(5) "Nikon"
samsung.jpg: NULL
panasonic.jpg: string(9) "Panasonic"
