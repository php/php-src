--TEST--
Bug #69948 (path/domain are not sanitized for special characters in setcookie)
--FILE--
<?php
try {
    var_dump(setcookie('foo', 'bar', 0, 'asdf;asdf'));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(setcookie('foo', 'bar', 0, '/', 'foobar; secure'));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
===DONE===
--EXPECTHEADERS--
--EXPECT--
ValueError: setcookie(): "path" option cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
ValueError: setcookie(): "domain" option cannot contain ",", ";", " ", "\t", "\r", "\n", "\013", or "\014"
===DONE===
