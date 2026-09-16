--TEST--
Prevent cloning of Variant types as it's broken
--EXTENSIONS--
com_dotnet
--FILE--
<?php

$v = new variant("123");

try {
	$v2 = clone $v;
    var_dump($v2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Trying to clone an uncloneable object of class variant
