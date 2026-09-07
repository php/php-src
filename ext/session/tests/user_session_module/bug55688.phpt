--TEST--
Bug #55688 (Crash when calling SessionHandler::gc())
--EXTENSIONS--
session
--INI--
html_errors=0
session.save_handler=files
--FILE--
<?php
ini_set('session.save_handler', 'files');
$x = new SessionHandler;

try {
    $x->gc(1);
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
Error: Session is not active
