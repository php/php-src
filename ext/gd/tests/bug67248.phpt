--TEST--
Bug #67248 (imageaffinematrixget missing check of parameters)
--SKIPIF--
<?php
        if(!extension_loaded('gd')){ die('skip gd extension not available'); }
        if(!function_exists('imageaffinematrixget')) die('skip imageaffinematrixget() not available');
?>
--FILE--
<?php
require __DIR__ . '/func.inc';

for($i=0;$i<7;$i++) {
	trycatch_dump(
        fn() => imageaffinematrixget($i)
    );
}
?>
--EXPECT--
!! [TypeError] Array expected as options when using translate or scale
!! [TypeError] Array expected as options when using translate or scale
!! [TypeError] Number is expected as option when using rotate or shear
!! [TypeError] Number is expected as option when using rotate or shear
!! [TypeError] Number is expected as option when using rotate or shear
!! [ValueError] Invalid type for element 5
!! [ValueError] Invalid type for element 6
