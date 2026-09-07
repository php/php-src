--TEST--
ob_start(): consumed callback arg has low refcount
--EXTENSIONS--
zend_test
--FILE--
<?php

$counts = [];
ob_start(static function ($buffer) use (&$counts) {
    $counts[] = zend_test_refcount($buffer);
    return '';
});
echo 'abc';
ob_end_flush();

var_dump($counts);

?>
--EXPECT--
array(1) {
  [0]=>
  int(2)
}
