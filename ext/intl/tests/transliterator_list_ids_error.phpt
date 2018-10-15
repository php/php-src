--TEST--
Transliterator::listIDs (error)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

ini_set("intl.error_level", E_WARNING);
var_dump(transliterator_list_ids(array()));

echo "Done.\n";
--EXPECTF--
Warning: transliterator_list_ids() expects exactly 0 parameters, 1 given in %s on line %d

Warning: transliterator_list_ids(): transliterator_list_ids: bad arguments in %s on line %d
bool(false)
Done.
