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
        fn() => imageaffinematrixget($i, new stdClass())
    );
}
?>
--EXPECTF--
!! [TypeError] imageaffinematrixget(): Argument #1 ($type) must be of type array when using translate or scale
!! [TypeError] imageaffinematrixget(): Argument #1 ($type) must be of type array when using translate or scale

Warning: Object of class stdClass could not be converted to float in %s on line %d
array(6) {
  [0]=>
  float(%f)
  [1]=>
  float(%f)
  [2]=>
  float(%f)
  [3]=>
  float(%f)
  [4]=>
  float(0)
  [5]=>
  float(0)
}

Warning: Object of class stdClass could not be converted to float in %s on line %d
array(6) {
  [0]=>
  float(1)
  [1]=>
  float(0)
  [2]=>
  float(%f)
  [3]=>
  float(1)
  [4]=>
  float(0)
  [5]=>
  float(0)
}

Warning: Object of class stdClass could not be converted to float in %s on line %d
array(6) {
  [0]=>
  float(1)
  [1]=>
  float(%f)
  [2]=>
  float(0)
  [3]=>
  float(1)
  [4]=>
  float(0)
  [5]=>
  float(0)
}
!! [ValueError] imageaffinematrixget(): Argument #1 ($type) must be a valid element type
!! [ValueError] imageaffinematrixget(): Argument #1 ($type) must be a valid element type
