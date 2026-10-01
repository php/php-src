--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - soft errors assigned to a typed property with a base URL
--FILE--
<?php

class Foo {
    public string $errors = "unchanged";
}

$foo = new Foo();
$builder = new Uri\WhatWg\UrlBuilder();
$builder->setFragment("a\tb");
try {
    $builder->build(new Uri\WhatWg\Url("https://example.com/"), $foo->errors);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($foo->errors);

?>
--EXPECT--
TypeError: Cannot assign array to reference held by property Foo::$errors of type string
string(9) "unchanged"
