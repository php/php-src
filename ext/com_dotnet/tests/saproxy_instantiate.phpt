--TEST--
Manual instantiation of com_safearray_proxy is not allowed
--EXTENSIONS--
com_dotnet
--FILE--
<?php
try {
    new com_safearray_proxy();
} catch (Error $e) {
    echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Cannot directly construct com_safeproxy_array; it is for internal usage only
