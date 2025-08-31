--TEST--
zip_entry_compressionmethod() function
--EXTENSIONS--
zip
--FILE--
<?php
$zip = zip_open(__DIR__."/test_procedural.zip");
if (!is_resource($zip)) die("Failure");
$entries = 0;
while ($entry = zip_read($zip)) {
  echo zip_entry_compressionmethod($entry)."\n";
}
zip_close($zip);

?>
--EXPECTF--
Deprecated: Function zip_open() is deprecated since 8.0, use ZipArchive::open() instead in %s on line %d

Deprecated: Function zip_read() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d

Deprecated: Function zip_entry_compressionmethod() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d
stored

Deprecated: Function zip_read() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d

Deprecated: Function zip_entry_compressionmethod() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d
stored

Deprecated: Function zip_read() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d

Deprecated: Function zip_entry_compressionmethod() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d
stored

Deprecated: Function zip_read() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d

Deprecated: Function zip_entry_compressionmethod() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d
deflated

Deprecated: Function zip_read() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d

Deprecated: Function zip_close() is deprecated since 8.0, use ZipArchive::close() instead in %s on line %d
