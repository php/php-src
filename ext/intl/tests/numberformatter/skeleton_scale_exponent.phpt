--TEST--
IntlNumberFormatter::createFromSkeleton() rejects a scale option in exponent notation
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (!class_exists('IntlNumberFormatter')) {
    die('skip IntlNumberFormatter not available');
}
?>
--INI--
memory_limit=16M
--FILE--
<?php

var_dump(IntlNumberFormatter::createFromSkeleton('precision-unlimited scale/1000', 'en_US')->format(1));
var_dump(IntlNumberFormatter::createFromSkeleton('scale/0.5 percent', 'en_US')->format(1));

foreach (['precision-unlimited scale/1e3000000', 'precision-unlimited scale/1E-3000000', 'scale/1.5e3'] as $skeleton) {
    var_dump(IntlNumberFormatter::createFromSkeleton($skeleton, 'en_US'));
    var_dump(intl_get_error_message());
}

ini_set('intl.use_exceptions', '1');

try {
    IntlNumberFormatter::createFromSkeleton('precision-unlimited scale/1e3000000', 'en_US');
} catch (IntlException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
string(5) "1,000"
string(4) "0.5%"
NULL
string(107) "IntlNumberFormatter::createFromSkeleton(): Number formatter creation failed: U_NUMBER_SKELETON_SYNTAX_ERROR"
NULL
string(107) "IntlNumberFormatter::createFromSkeleton(): Number formatter creation failed: U_NUMBER_SKELETON_SYNTAX_ERROR"
NULL
string(107) "IntlNumberFormatter::createFromSkeleton(): Number formatter creation failed: U_NUMBER_SKELETON_SYNTAX_ERROR"
IntlException: IntlNumberFormatter::createFromSkeleton(): Number formatter creation failed
