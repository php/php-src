--TEST--
zip_read() function
--EXTENSIONS--
zip
--FILE--
<?php
$zip = zip_open(__DIR__."/test_procedural.zip");
if (!is_resource($zip)) die("Failure");
$entries = 0;
while ($entry = zip_read($zip)) {
  $entries++;
}
zip_close($zip);
echo "$entries entries";

?>
--EXPECTF--
Deprecated: Function zip_open() is deprecated since 8.0, use ZipArchive::open() instead in %s on line %d

Deprecated: Function zip_read() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d

Deprecated: Function zip_read() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d

Deprecated: Function zip_read() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d

Deprecated: Function zip_read() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d

Deprecated: Function zip_read() is deprecated since 8.0, use ZipArchive::statIndex() instead in %s on line %d

Deprecated: Function zip_close() is deprecated since 8.0, use ZipArchive::close() instead in %s on line %d
4 entries
