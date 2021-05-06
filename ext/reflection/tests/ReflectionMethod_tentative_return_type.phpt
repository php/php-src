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
echo "\n";

$methodInfo = new ReflectionMethod(MyDateTimeZone::class, 'listIdentifiers');

var_dump($methodInfo->hasReturnType());
var_dump($methodInfo->hasTentativeReturnType());
var_dump((string) $methodInfo->getReturnType());
var_dump($methodInfo->getTentativeReturnType());
echo "\n";

?>
--EXPECTF--
bool(false)
bool(true)
NULL
string(5) "array"

bool(true)
bool(false)
string(6) "string"
NULL
