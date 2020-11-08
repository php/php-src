--TEST--
Bug #78182: Segmentation fault during by-reference property assignment
--FILE--
<?php
$varName = 'var';
$propName = 'prop';
try {
    $$varName->$propName =& $$varName;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($var);
?>
--EXPECT--
Attempt to modify property "prop" on null
NULL
