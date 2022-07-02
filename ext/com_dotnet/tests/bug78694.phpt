--TEST--
Bug #78694 (Appending to a variant array causes segfault)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
foreach ([new com('WScript.Shell'), new variant([])] as $var) {
    try {
        $var[] = 42;
    } catch (com_exception $ex) {
        var_dump($ex->getMessage());
    }
}
?>
--EXPECT--
string(38) "appending to variants is not supported"
string(38) "appending to variants is not supported"
