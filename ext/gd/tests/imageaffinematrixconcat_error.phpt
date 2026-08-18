--TEST--
imageaffinematrixconcat() reports the correct argument for an invalid matrix size
--EXTENSIONS--
gd
--FILE--
<?php
try {
    imageaffinematrixconcat([1, 0, 0, 1, 0, 0], []);
} catch (ValueError $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: imageaffinematrixconcat(): Argument #2 ($matrix2) must have 6 elements
