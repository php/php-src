--TEST--
SKIPIF and CLEAN receive test-specific extra arguments
--INI--
error_reporting=123
--SKIPIF--
<?php
$marker = __DIR__ . '/auxiliary_environment.tmp';
if (str_contains(getenv('TEST_PHP_EXTRA_ARGS'), 'error_reporting=123')) {
    file_put_contents($marker, "set\n");
}
?>
--FILE--
<?php
$marker = __DIR__ . '/auxiliary_environment.tmp';
echo file_get_contents($marker);
?>
--CLEAN--
<?php
$marker = __DIR__ . '/auxiliary_environment.tmp';
if (!str_contains(getenv('TEST_PHP_EXTRA_ARGS'), 'error_reporting=123')) {
    echo "CLEAN did not receive TEST_PHP_EXTRA_ARGS\n";
}
@unlink($marker);
?>
--EXPECT--
set
