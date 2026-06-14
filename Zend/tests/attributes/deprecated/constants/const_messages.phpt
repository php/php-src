--TEST--
#[\Deprecated]: Messages on compile time constants.
--FILE--
<?php

#[\Deprecated]
const DeprecatedConst1 = 1;

#[\Deprecated("use DEPRECATED_CONST_2")]
const DeprecatedConst2 = 2;

#[\Deprecated(message: "use DEPRECATED_CONST_3")]
const DeprecatedConst3 = 3;

#[\Deprecated(message: "use DEPRECATED_CONST_4", since: "1.0")]
const DeprecatedConst4 = 4;

#[\Deprecated("use DEPRECATED_CONST_5", "1.0")]
const DeprecatedConst5 = 5;

#[\Deprecated(since: "1.0")]
const DeprecatedConst6 = 6;

echo DeprecatedConst1 . "\n";
echo DeprecatedConst2 . "\n";
echo DeprecatedConst3 . "\n";
echo DeprecatedConst4 . "\n";
echo DeprecatedConst5 . "\n";
echo DeprecatedConst6 . "\n";
?>
--EXPECTF--
Deprecated: Constant DeprecatedConst1 is deprecated in %s on line %d
1

Deprecated: Constant DeprecatedConst2 is deprecated, use DEPRECATED_CONST_2 in %s on line %d
2

Deprecated: Constant DeprecatedConst3 is deprecated, use DEPRECATED_CONST_3 in %s on line %d
3

Deprecated: Constant DeprecatedConst4 is deprecated since 1.0, use DEPRECATED_CONST_4 in %s on line %d
4

Deprecated: Constant DeprecatedConst5 is deprecated since 1.0, use DEPRECATED_CONST_5 in %s on line %d
5

Deprecated: Constant DeprecatedConst6 is deprecated since 1.0 in %s on line %d
6

