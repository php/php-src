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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($list->value);

?>
--EXPECT--
string(0) ""
ValueError: Value must not contain any null bytes
string(0) ""
