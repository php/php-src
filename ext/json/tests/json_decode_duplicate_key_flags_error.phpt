--TEST--
json_decode() rejects combining JSON_DUPLICATE_KEY_MERGE and JSON_DUPLICATE_KEY_ARRAY
--FILE--
<?php
try {
    json_decode('{}', true, 512, JSON_DUPLICATE_KEY_MERGE | JSON_DUPLICATE_KEY_ARRAY);
    echo "no throw\n";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
--EXPECT--
json_decode(): Argument #4 ($flags) cannot combine JSON_DUPLICATE_KEY_MERGE and JSON_DUPLICATE_KEY_ARRAY
