--TEST--
Bug #7214 (zip_entry_read() binary safe)
--EXTENSIONS--
zip
--FILE--
<?php
$zip = zip_open(__DIR__."/binarynull.zip");
if (!is_resource($zip)) die("Failure");
$entries = 0;
$entry = zip_read($zip);
$contents = zip_entry_read($entry, zip_entry_filesize($entry));
if (strlen($contents) == zip_entry_filesize($entry)) {
    echo "Ok";
} else {
    echo "failed";
}

?>
--EXPECTF--
Deprecated: Function zip_open() is deprecated in %s on line %d

Deprecated: Function zip_read() is deprecated in %s on line %d

Deprecated: Function zip_entry_filesize() is deprecated in %s on line %d

Deprecated: Function zip_entry_read() is deprecated in %s on line %d

Deprecated: Function zip_entry_filesize() is deprecated in %s on line %d
Ok
