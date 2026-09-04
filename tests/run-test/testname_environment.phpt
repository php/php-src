--TEST--
TEST_NAME placeholder in --ENV--
--ENV--
TEST_PWD={PWD}
TEST_NAME={TEST_NAME}
--FILE--
<?php
echo str_replace('\\', '/', getenv('TEST_PWD') . '/' . getenv('TEST_NAME')), PHP_EOL;
?>
--EXPECTF--
%stests/run-test/testname_environment
