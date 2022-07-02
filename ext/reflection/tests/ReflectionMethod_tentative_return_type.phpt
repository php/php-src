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

function printInfo(ReflectionMethod $methodInfo) {
    var_dump($methodInfo->hasReturnType());
    var_dump($methodInfo->hasTentativeReturnType());
    var_dump((string) $methodInfo->getReturnType());
    var_dump((string) $methodInfo->getTentativeReturnType());
    var_dump((string) $methodInfo);
    echo "\n";
}

printInfo(new ReflectionMethod(DateTimeZone::class, 'listIdentifiers'));
printInfo(new ReflectionMethod(MyDateTimeZone::class, 'listIdentifiers'));
printInfo(new ReflectionMethod(FileSystemIterator::class, 'current'));

?>
--EXPECTF--
bool(false)
bool(true)
string(0) ""
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
string(0) ""
string(%d) "Method [ <user, overwrites DateTimeZone, prototype DateTimeZone> static public method listIdentifiers ] {
  @@ %s

  - Parameters [2] {
    Parameter #0 [ <optional> int $timezoneGroup = DateTimeZone::ALL ]
    Parameter #1 [ <optional> ?string $countryCode = NULL ]
  }
  - Return [ string ]
}
"

bool(false)
bool(true)
string(0) ""
string(37) "SplFileInfo|FilesystemIterator|string"
string(191) "Method [ <internal:SPL, overwrites DirectoryIterator, prototype Iterator> public method current ] {

  - Parameters [0] {
  }
  - Tentative return [ SplFileInfo|FilesystemIterator|string ]
}
"
