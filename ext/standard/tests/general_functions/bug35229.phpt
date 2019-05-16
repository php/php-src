--TEST--
Bug #35229 (call_user_func() crashes when argument stack is nearly full)
--FILE--
<?php
class test2 {
  static function use_stack() {
    echo "OK\n";
  }
}

spl_autoload_register(function ($class) {
	eval('class test1 extends test2 {}');

	test1::use_stack(
    1,2,3,4,5,6,7,8,9,10,
    11,12,13,14,15,16,17,18,19,20,
    21,22,23,24,25,26,27,28,29,30
  );
});

call_user_func(array('test1', 'use_stack'),
  1,2,3,4,5,6,7,8,9,10,
  11,12,13,14,15,16,17,18,19,20,
  21,22,23,24,25,26,27,28,29,30
);
?>
--EXPECT--
OK
OK
