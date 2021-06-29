--TEST--
Testing repated SessionHandler::open() calls
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ini_set('session.save_handler', 'files');
$x = new SessionHandler;

try {
    $x->open('','');
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $x->open('','');
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $x->open('','');
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $x->open('','');
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

print "Done!\n";

?>
--EXPECT--
Session is not active
Session is not active
Session is not active
Session is not active
Done!
