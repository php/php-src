--TEST--
zip_entry_close() function: simple and double call
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$zip    = zip_open(__DIR__."/test_procedural.zip");
$entry  = zip_read($zip);
echo "entry_open:  "; var_dump(zip_entry_open($zip, $entry, "r"));
echo "entry_close: "; var_dump(zip_entry_close($entry));
try {
    echo "entry_close: "; var_dump(zip_entry_close($entry));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
zip_close($zip);
?>
Done
--EXPECT--
entry_open:  bool(true)
entry_close: bool(true)
entry_close: zip_entry_close(): supplied resource is not a valid Zip Entry resource
Done
