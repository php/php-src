--TEST--
com_safearray_proxy errors
--EXTENSIONS--
com_dotnet
--FILE--
<?php

$binding_string = ['aaa','bbb','ccc'];
$v = new VARIANT( $binding_string, VT_ARRAY );
var_dump($v);

try {
    var_dump(isset($v->hello));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump($v->hello);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $v->hello = 'blah';
    var_dump($v);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    unset($v->hello);
    var_dump($v);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $c = clone $v;
    var_dump($c);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $s = serialize($v);
    var_dump($s);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
object(variant)#1 (0) {
}
bool(false)
com_exception: this variant has no properties
com_exception: this variant has no properties
Error: Cannot delete properties from a COM object
Error: Trying to clone an uncloneable object of class variant
Exception: Serialization of 'variant' is not allowed
