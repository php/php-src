--TEST--
ZE2: set_exception_handler()
--SKIPIF--
<?php if (version_compare(zend_version(), "2", "<")) print "skip"; ?>
--FILE--
<?php

set_exception_handler("my_handler");
try {
    throw new exception();
} catch (stdClass $e) {
    print "BAR\n";
}

function my_handler($exception) {
    print "FOO\n";
}

?>
--EXPECT--
FOO
