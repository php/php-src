--TEST--
Bug #67248 (imageaffinematrixget missing check of parameters)
--SKIPIF--
<?php
        if(!extension_loaded('gd')){ die('skip gd extension not available'); }
        if(!function_exists('imageaffinematrixget')) die('skip imageaffinematrixget() not available');
?>
--FILE--
<?php
for($i=0;$i<7;$i++) {
	imageaffinematrixget($i);
}
?>
--EXPECTF--
Warning: imageaffinematrixget(): Array expected as options in %s on line %d

Warning: imageaffinematrixget(): Array expected as options in %s on line %d

Warning: imageaffinematrixget(): Number is expected as option in %s on line %d

Warning: imageaffinematrixget(): Number is expected as option in %s on line %d

Warning: imageaffinematrixget(): Number is expected as option in %s on line %d

Warning: imageaffinematrixget(): Invalid type for element 5 in %s on line %d

Warning: imageaffinematrixget(): Invalid type for element 6 in %s on line %d
