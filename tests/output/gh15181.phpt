--TEST--
Fix GH-15181 (Disabled output handler is flushed again)
--FILE--
<?php
ob_start(function () {
    throw new Exception('ob_start');
});
try {
    ob_flush();
} catch (Throwable) {}
ob_flush();
?>
===DONE===
--EXPECT--
===DONE===
