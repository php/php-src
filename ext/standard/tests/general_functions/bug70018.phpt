--TEST--
Bug #70018 (exec does not strip all whitespace)
--FILE--
<?php
$output = array();

$test_fl = __DIR__ . DIRECTORY_SEPARATOR . md5(uniqid());
$test_fl_escaped = escapeshellarg($test_fl);
file_put_contents($test_fl, '<?php echo "abc\f\n \n";');

exec(getenv('TEST_PHP_EXECUTABLE_ESCAPED') . " -n $test_fl_escaped", $output);

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
