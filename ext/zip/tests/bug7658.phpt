--TEST--
Bug #7658 (modify archive with general bit flag 3 set)
--EXTENSIONS--
zip
--FILE--
<?php
$expect = array(
    "mimetype",
    "Configurations2/statusbar/",
    "Configurations2/accelerator/current.xml",
    "Configurations2/floater/",
    "Configurations2/popupmenu/",
    "Configurations2/progressbar/",
    "Configurations2/menubar/",
    "Configurations2/toolbar/",
    "Configurations2/images/Bitmaps/",
    "content.xml",
    "styles.xml",
    "meta.xml",
    "Thumbnails/thumbnail.png",
    "settings.xml",
    "META-INF/manifest.xml",
);
$dirname = __DIR__ . '/';
include $dirname . 'utils.inc';
$file = $dirname . '__tmp_bug7658.odt';
$zip = new ZipArchive();
copy($dirname . 'bug7658.odt', $file);
if(!$zip->open($file)) {
    echo 'failed';
}


$zip->deleteName('content.xml');
$zip->addFile($dirname . "bug7658.xml","content.xml");
$zip->close();
echo "\n";
$zip->open($file);

for($i=0; $i < $zip->numFiles; $i++) {
    $sb = $zip->statIndex($i);
    $found[] = $sb['name'];
}
$ar = array_diff($found, $expect);

var_dump($ar);
unset($zip);
unlink($file);
?>
--EXPECT--
array(0) {
}
