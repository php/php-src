--TEST--
imageaffinematrixconcat() reports the correct argument for an invalid matrix size
--EXTENSIONS--
gd
--FILE--
<?php
try {
    imageaffinematrixconcat([1, 0, 0, 1, 0, 0], []);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imageaffinematrixconcat(): Argument #2 ($matrix2) must have 6 elements
