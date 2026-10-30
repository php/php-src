--TEST--
Test session_set_save_handler() function : error functionality
--EXTENSIONS--
session
--INI--
session.save_handler=
--FILE--
<?php
try {
    session_set_save_handler(new \SessionHandler(), true);
    session_start();
} catch (Throwable $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}
echo "ok";
?>
--EXPECT--
Error: Cannot call default session handler
ok
