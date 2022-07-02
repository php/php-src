--TEST--
Bug #78396: Second file_put_contents in Shutdown hangs script
--FILE--
<?php

register_shutdown_function(function () {
    file_put_contents(__DIR__ . '/bug78396.txt', '1', FILE_APPEND | LOCK_EX);
    file_put_contents(__DIR__ . '/bug78396.txt', '2', FILE_APPEND | LOCK_EX);
    echo "Done\n";
});

?>
--CLEAN--
<?php
unlink(__DIR__ . '/bug78396.txt');
?>
--EXPECT--
Done
