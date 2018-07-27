--TEST--
Bug #76275: Assertion failure in file cache when unserializing empty try_catch_array
--INI--
opcache.enabled=1
opcache.enable_cli=1
opcache.file_cache=/tmp
--FILE--
<?php
if (!is_callable('random_bytes')) {
    try {
    } catch (com_exception $e) {
    }

    function random_bytes($length)
    {
        throw new Exception(
            'There is no suitable CSPRNG installed on your system'
        );
    }
}
echo 'Done';
--EXPECT--
Done
