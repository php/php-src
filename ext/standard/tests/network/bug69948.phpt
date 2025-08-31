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
setcookie(): "path" option cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
setcookie(): "domain" option cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
===DONE===
