--TEST--
fclose() actually closes streams with refcount > 1
--FILE--
<?php
$s = fopen(__FILE__, "rb");
function separate_zval(&$var) { }
$s2 = $s;
separate_zval($s2);
fclose($s);
try {
    echo fread($s2, strlen("<?php"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
echo "\nDone.\n";
?>
--EXPECT--
TypeError: fread(): Argument #1 ($stream) must be an open stream resource

Done.
