--TEST--
zip_entry_close() function: simple and double call
--EXTENSIONS--
zip
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
--EXPECTF--
Deprecated: Function zip_open() is deprecated in %s on line %d

Deprecated: Function zip_read() is deprecated in %s on line %d
entry_open:  
Deprecated: Function zip_entry_open() is deprecated in %s on line %d
bool(true)
entry_close: 
Deprecated: Function zip_entry_close() is deprecated in %s on line %d
bool(true)
entry_close: 
Deprecated: Function zip_entry_close() is deprecated in %s on line %d
zip_entry_close(): supplied resource is not a valid Zip Entry resource

Deprecated: Function zip_close() is deprecated in %s on line %d
Done
