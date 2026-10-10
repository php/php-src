--TEST--
Test Csv\collection_to_buffer(): a Generator is destroyed immediately when conversion fails
--EXTENSIONS--
csv
--INI--
; the exception backtrace would otherwise keep a reference to the generator argument
zend.exception_ignore_args=1
--FILE--
<?php
function rows(): Generator {
    try {
        yield ['a', 'b'];
        yield ['c'];
    } finally {
        echo "generator destroyed", \PHP_EOL;
    }
}

try {
    Csv\collection_to_buffer(rows());
} catch (ValueError $e) {
    echo $e::class, \PHP_EOL;
}
echo "after catch", \PHP_EOL;
?>
--EXPECT--
generator destroyed
ValueError
after catch
