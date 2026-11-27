--TEST--
A failing request_begin hook keeps the script from running and fails the process
--EXTENSIONS--
zend_test
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$args = getenv('TEST_PHP_EXTRA_ARGS') . ' -d display_errors=1 -d zend_test.request_begin_fails=1';
$script = __DIR__ . '/request_hooks_begin_failure.inc';
file_put_contents($script, '<?php echo "script ran", PHP_EOL;');

exec("$php $args " . escapeshellarg($script), $output, $status);
var_dump($output, $status);

$output = [];
exec("$php $args -r " . escapeshellarg('echo 1, PHP_EOL;'), $output, $status);
var_dump($output, $status);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/request_hooks_begin_failure.inc');
?>
--EXPECTF--
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(%d) "Warning: request_begin() for zend_test module failed in %s"
}
int(255)
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(%d) "Warning: request_begin() for zend_test module failed in %s"
}
int(255)
