--TEST--
ArrayObject illegal offset
--FILE--
<?php

$ao = new ArrayObject([1, 2, 3]);
try {
    var_dump($ao[[]]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $ao[[]] = new stdClass;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    $ref =& $ao[[]];
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(isset($ao[[]]));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    unset($ao[[]]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Illegal offset type
Illegal offset type
Illegal offset type
Illegal offset type in isset or empty
Illegal offset type in unset
