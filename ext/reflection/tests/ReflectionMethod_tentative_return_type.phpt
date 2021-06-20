--TEST--
ReflectionMethod returns tentative return type information correctly
--FILE--
<?php

class MyDateTimeZone extends DateTimeZone
{
    #[ReturnTypeWillChange]
    public static function listIdentifiers(int $timezoneGroup = DateTimeZone::ALL, ?string $countryCode = null): string
    {
        return "";
    }
}

$methodInfo = new ReflectionMethod(DateTimeZone::class, 'listIdentifiers');

var_dump($methodInfo->hasReturnType());
var_dump($methodInfo->hasTentativeReturnType());
var_dump($methodInfo->getReturnType());
var_dump((string) $methodInfo->getTentativeReturnType());
var_dump((string) $methodInfo);
echo "\n";

$methodInfo = new ReflectionMethod(MyDateTimeZone::class, 'listIdentifiers');

var_dump($methodInfo->hasReturnType());
var_dump($methodInfo->hasTentativeReturnType());
var_dump((string) $methodInfo->getReturnType());
var_dump($methodInfo->getTentativeReturnType());
var_dump((string) $methodInfo);
echo "\n";

?>
--EXPECTF--
bool(false)
bool(true)
NULL
string(5) "array"
string(%d) "Method [ <internal:date> static public method listIdentifiers ] {

  - Parameters [2] {
    Parameter #0 [ <optional> int $timezoneGroup = DateTimeZone::ALL ]
    Parameter #1 [ <optional> ?string $countryCode = null ]
  }
  - Tentative return [ array ]
}
"

bool(true)
bool(false)
string(6) "string"
NULL
string(%d) "Method [ <user, overwrites DateTimeZone, prototype DateTimeZone> static public method listIdentifiers ] {
  @@ %s

  - Parameters [2] {
    Parameter #0 [ <optional> int $timezoneGroup = %d ]
    Parameter #1 [ <optional> ?string $countryCode = NULL ]
  }
  - Return [ string ]
}
"
