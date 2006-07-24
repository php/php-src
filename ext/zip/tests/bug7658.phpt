--TEST--
bug #7658, modify archive with general bit flag 3 set
--SKIPIF--
<?php
/* $Id$ */
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$dirname = dirname(__FILE__) . '/';
include $dirname . 'utils.php';
$file = $dirname . '__tmp_bug7658.odt';
$zip = new ZipArchive();
copy($dirname . 'bug7658.odt', $file);
if(!$zip->open($file)) {
	echo 'failed';
}

dump_entries_name($zip);

$zip->deleteName('content.xml');
$zip->addFile($dirname . "bug7658.xml","content.xml");
$zip->close();
echo "\n";
$zip->open($file);
dump_entries_name($zip);
@unlink($file);
?>
--EXPECT--
0 mimetype
1 Configurations2/statusbar/
2 Configurations2/accelerator/current.xml
3 Configurations2/floater/
4 Configurations2/popupmenu/
5 Configurations2/progressbar/
6 Configurations2/menubar/
7 Configurations2/toolbar/
8 Configurations2/images/Bitmaps/
9 content.xml
10 styles.xml
11 meta.xml
12 Thumbnails/thumbnail.png
13 settings.xml
14 META-INF/manifest.xml

0 mimetype
1 Configurations2/statusbar/
2 Configurations2/accelerator/current.xml
3 Configurations2/floater/
4 Configurations2/popupmenu/
5 Configurations2/progressbar/
6 Configurations2/menubar/
7 Configurations2/toolbar/
8 Configurations2/images/Bitmaps/
9 styles.xml
10 meta.xml
11 Thumbnails/thumbnail.png
12 settings.xml
13 META-INF/manifest.xml
14 content.xml
