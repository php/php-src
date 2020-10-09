--TEST--
Bug #67314 (Segmentation fault in gc_remove_zval_from_buffer)
--FILE--
<?php
function crash()
{
    $notDefined[$i] = 'test';
}

function error_handler() { return false; }

set_error_handler('error_handler');
crash();
echo "made it once\n";
crash();
echo "ok\n";
?>
--EXPECTF--
Warning: Undefined variable $i in %s on line %d
made it once

Warning: Undefined variable $i in %s on line %d
ok
