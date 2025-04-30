--TEST--
TokenList: value edge cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root/>');
$list = $dom->documentElement->classList;

var_dump($list->value);

try {
    $list->value = "\0";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

var_dump($list->value);

?>
--EXPECT--
string(0) ""
Value must not contain any null bytes
string(0) ""
