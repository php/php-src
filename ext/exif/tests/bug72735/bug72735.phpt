--TEST--
Bug #72735 (Samsung picture thumb not read (zero size))
--EXTENSIONS--
exif
--FILE--
<?php
foreach (['nokia.jpg', 'samsung.jpg', 'panasonic.jpg'] as $picture) {
    echo $picture . ': ';

    $len = strlen(exif_thumbnail(__DIR__ . DIRECTORY_SEPARATOR . $picture));

    if (!$len) {
        echo 'Error, no length returned', PHP_EOL;

        continue;
    }

    echo 'int(' . $len . ')', PHP_EOL;
}
?>
--EXPECT--
nokia.jpg: int(5899)
samsung.jpg: int(5778)
panasonic.jpg: int(651)
