--TEST--
imageaffinematrixconcat() reports the correct argument for an invalid matrix size
--EXTENSIONS--
gd
--FILE--
<?php
try {
    imageaffinematrixconcat([1, 0, 0, 1, 0, 0], []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: imageaffinematrixconcat(): Argument #2 ($matrix2) must have 6 elements
