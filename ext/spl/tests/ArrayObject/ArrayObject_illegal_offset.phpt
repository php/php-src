--TEST--
ArrayObject illegal offset
--FILE--
<?php

$ao = new ArrayObject([1, 2, 3]);
try {
    var_dump($ao[[]]);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $ao[[]] = new stdClass;
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $ref =& $ao[[]];
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(isset($ao[[]]));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unset($ao[[]]);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Cannot access offset of type array on ArrayObject
TypeError: Cannot access offset of type array on ArrayObject
TypeError: Cannot access offset of type array on ArrayObject
TypeError: Cannot access offset of type array in isset or empty
TypeError: Cannot unset offset of type array on ArrayObject
