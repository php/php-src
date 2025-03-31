--TEST--
Bug #67248 (imageaffinematrixget missing check of parameters)
--EXTENSIONS--
gd
--SKIPIF--
<?php
        if(!function_exists('imageaffinematrixget')) die('skip imageaffinematrixget() not available');
?>
--FILE--
<?php
require __DIR__ . '/func.inc';

for($i=0;$i<7;$i++) {
    trycatch_dump(
        fn() => imageaffinematrixget($i, new stdClass())
    );
}
?>
--EXPECTF--
!! [TypeError] imageaffinematrixget(): Argument #1 ($type) must be of type array when using translate or scale
!! [TypeError] imageaffinematrixget(): Argument #1 ($type) must be of type array when using translate or scale
!! [TypeError] imageaffinematrixget(): Argument #2 ($options) must be of type float, stdClass given
!! [TypeError] imageaffinematrixget(): Argument #2 ($options) must be of type float, stdClass given
!! [TypeError] imageaffinematrixget(): Argument #2 ($options) must be of type float, stdClass given
!! [ValueError] imageaffinematrixget(): Argument #1 ($type) must be a valid element type
!! [ValueError] imageaffinematrixget(): Argument #1 ($type) must be a valid element type
