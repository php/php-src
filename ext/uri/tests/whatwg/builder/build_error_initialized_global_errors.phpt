--TEST--
Test Uri\WhatWg\UrlBuilder::build() - error - initializes errors for invalid component combinations
--FILE--
<?php

$builder = new Uri\WhatWg\UrlBuilder();
$builder->setScheme("foo");
$builder->setUsername("user");

try {
    $builder->build();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
    var_dump($e->errors);
}

?>
--EXPECT--
Uri\WhatWg\InvalidUrlException: The specified URL cannot have username
array(0) {
}
