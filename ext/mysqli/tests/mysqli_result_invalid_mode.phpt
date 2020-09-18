--TEST--
mysqli_result(), invalid mode
--SKIPIF--
<?php
require_once('skipif.inc');
require_once('skipifconnectfailure.inc');
?>
--FILE--
<?php
    require('connect.inc');
    require('table.inc');

    $valid = array(MYSQLI_STORE_RESULT, MYSQLI_USE_RESULT);
    $invalidModes = [-1, 152];
    foreach ($invalidModes as $mode) {
        try {
            new mysqli_result($link, $mode);
        } catch (\ValueError $e) {
            echo $e->getMessage() . \PHP_EOL;
        }
    }
?>
--CLEAN--
<?php
    require_once("clean_table.inc");
?>
--EXPECT--
mysqli_result::__construct(): Argument #2 ($result_mode) must be either MYSQLI_STORE_RESULT or MYSQLI_USE_RESULT
mysqli_result::__construct(): Argument #2 ($result_mode) must be either MYSQLI_STORE_RESULT or MYSQLI_USE_RESULT
