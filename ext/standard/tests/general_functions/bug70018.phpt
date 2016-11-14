--TEST--
Bug #70018 (exec does not strip all whitespace)
--SKIPIF--
<?php
if ("cli" != php_sapi_name()) {
  die("skip.. CLI only test");
}
?>
--FILE--
<?php
$output = array();

$test_fl = __DIR__ . DIRECTORY_SEPARATOR . md5(uniqid());
file_put_contents($test_fl, '<?php echo "abc\f\n \n";');

exec(PHP_BINARY . " -n $test_fl", $output);

var_dump($output);

@unlink($test_fl);
?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "abc"
  [1]=>
  string(0) ""
}
