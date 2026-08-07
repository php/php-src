--TEST--
Bug #78182: Segmentation fault during by-reference property assignment
--FILE--
<?php
$varName = 'var';
$propName = 'prop';
try {
    $$varName->$propName =& $$varName;
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($var);
?>
--EXPECT--
Error: Attempt to modify property "prop" on null
NULL
