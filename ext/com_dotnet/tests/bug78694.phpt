--TEST--
Bug #78694 (Appending to a variant array causes segfault)
--SKIPIF--
<?php
if (!extension_loaded('com_dotnet')) die('skip com_dotnet extension not available');
?>
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
