--TEST--
GH-23842: skipLazyInitialization() copies an unresolved constant default with opcache
--CREDITS--
DirkTrunkstar
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
--FILE--
<?php

class Currency {
    public const string EUR = 'EUR';
}

class Product {
    public string $currency = Currency::EUR;
}

$reflector = new ReflectionClass(Product::class);
$product = $reflector->newLazyGhost(function () {
    throw new \Exception('initializer');
});
$reflector->getProperty('currency')->skipLazyInitialization($product);

var_dump($product->currency);

?>
--EXPECT--
string(3) "EUR"
