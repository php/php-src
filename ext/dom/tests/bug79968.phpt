--TEST--
dom: Bug #79968 - Crash when calling before without valid hierachy
--EXTENSIONS--
dom
--FILE--
<?php

$cdata = new DOMText;

try {
    $cdata->before("string");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $cdata->after("string");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $cdata->replaceWith("string");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Hierarchy Request Error
Hierarchy Request Error
Hierarchy Request Error
