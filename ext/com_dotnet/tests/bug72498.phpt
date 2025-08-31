--TEST--
Bug #72498 variant_date_from_timestamp null dereference
--EXTENSIONS--
com_dotnet
--FILE--
<?php

$v1 = PHP_INT_MAX;

try {
    var_dump(variant_date_from_timestamp($v1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
variant_date_from_timestamp(): Argument #1 ($timestamp) must not go past 23:59:59, December 31, 3000, UTC
