<?php
if(!extension_loaded('fileinfo')) {
	dl('fileinfo.' . PHP_SHLIB_SUFFIX);
}
if(!extension_loaded('fileinfo')) {
	die("fileinfo extension is not available, please compile it.\n");
}

// normal operation
$res = finfo_open(FILEINFO_MIME); /* return mime type ala mimetype extension */
$files = glob("*");
foreach ($files as $file) {
	echo finfo_file($res, $file) . "\n";
}
finfo_close($res);

// OO mode
/*
 * FILEINFO_PRESERVE_ATIME - if possible preserve the original access time
 * FILEINFO_SYMLINK - follow symlinks
 * FILEINFO_DEVICES - look at the contents of blocks or character special devices
 * FILEINFO_COMPRESS - decompress compressed files
 */
$fi = new finfo(FILEINFO_PRESERVE_ATIME|FILEINFO_SYMLINK|FILEINFO_DEVICES|FILEINFO_COMPRESS);
$files = glob("*");
foreach ($files as $file) {
        echo $fi->buffer(file_get_contents($file)) . "\n";
}
?>
