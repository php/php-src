--TEST--
php://memory and php://temp stream seek with PHP_INT_MIN does not trigger undefined behavior
--FILE--
<?php
foreach (['php://memory', 'php://temp'] as $wrapper) {
    echo "=== $wrapper ===\n";

    $stream = fopen($wrapper, 'r+');
    fwrite($stream, 'hello');

    // SEEK_CUR with PHP_INT_MIN from middle of stream
    fseek($stream, 2, SEEK_SET);
    var_dump(fseek($stream, PHP_INT_MIN, SEEK_CUR));

    // SEEK_CUR with PHP_INT_MIN from beginning of stream
    fseek($stream, 0, SEEK_SET);
    var_dump(fseek($stream, PHP_INT_MIN, SEEK_CUR));

    // SEEK_END with PHP_INT_MIN
    var_dump(fseek($stream, PHP_INT_MIN, SEEK_END));

    // Normal negative SEEK_CUR that should succeed
    fseek($stream, 4, SEEK_SET);
    var_dump(fseek($stream, -2, SEEK_CUR));
    var_dump(ftell($stream));

    // Normal negative SEEK_END that should succeed
    var_dump(fseek($stream, -3, SEEK_END));
    var_dump(ftell($stream));

    // Verify stream still works after all edge-case seeks
    fseek($stream, 0, SEEK_SET);
    var_dump(fread($stream, 5));

    fclose($stream);
}
echo "Done\n";
?>
--EXPECT--
=== php://memory ===
int(-1)
int(-1)
int(-1)
int(0)
int(2)
int(0)
int(2)
string(5) "hello"
=== php://temp ===
int(-1)
int(-1)
int(-1)
int(0)
int(2)
int(0)
int(2)
string(5) "hello"
Done
