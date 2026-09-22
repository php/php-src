--TEST--
Testing repated SessionHandler::open() calls
--EXTENSIONS--
session
--FILE--
<?php

ini_set('session.save_handler', 'files');
$x = new SessionHandler;

try {
    $x->open('','');
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    $x->open('','');
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    $x->open('','');
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    $x->open('','');
} catch (Error $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

print "Done!\n";

?>
--EXPECT--
Error: Session is not active
Error: Session is not active
Error: Session is not active
Error: Session is not active
Done!
