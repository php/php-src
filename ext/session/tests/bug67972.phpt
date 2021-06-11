--TEST--
Bug #67972: SessionHandler Invalid memory read create_sid()
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

try {
    (new SessionHandler)->create_sid();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Session is not active
