--TEST--
dl() / observer segfault
--EXTENSIONS--
zend_test
--SKIPIF--
<?php include dirname(__DIR__, 3) . "/dl_test/tests/skip.inc"; ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_functions=1
zend_test.observer.show_output=1
--FILE--
<?php

if (PHP_OS_FAMILY === 'Windows') {
    $loaded = dl('php_dl_test.dll');
} else {
    $loaded = dl('dl_test.so');
}

var_dump(dl_test_test2("World!"));

$test = new DlTest();
var_dump($test->test("World!"));
?>
--EXPECTF--
<!-- init '%sgh17211.php' -->
<!-- init dl() -->
<dl>
</dl>
<!-- init dl_test_test2() -->
<dl_test_test2>
</dl_test_test2>
<!-- init var_dump() -->
<var_dump>
string(12) "Hello World!"
</var_dump>
<!-- init DlTest::test() -->
<DlTest::test>
</DlTest::test>
<var_dump>
string(12) "Hello World!"
</var_dump>
