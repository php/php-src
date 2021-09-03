--TEST--
Transliterator::listIDs (basic)
--EXTENSIONS--
intl
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);
var_dump(count(transliterator_list_ids()) > 100);
var_dump(count(Transliterator::listIDs()) > 100);

echo "Done.\n";
?>
--EXPECT--
bool(true)
bool(true)
Done.
