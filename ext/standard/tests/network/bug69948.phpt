--TEST--
Bug #69948 (path/domain are not sanitized for special characters in setcookie)
--FILE--
<?php
try {
    var_dump(setcookie('foo', 'bar', 0, 'asdf;asdf'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(setcookie('foo', 'bar', 0, '/', 'foobar; secure'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
===DONE===
--EXPECTHEADERS--
--EXPECT--
setcookie(): Argument #4 ($path) cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", and "\014"
setcookie(): Argument #5 ($domain) cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", and "\014"
===DONE===
