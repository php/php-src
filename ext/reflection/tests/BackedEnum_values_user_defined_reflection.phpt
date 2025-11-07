--TEST--
BackedEnum: reflection shows whether values() is native or user-defined
--FILE--
<?php

// Native implementation
enum NativeEnum: string {
    case A = 'a';
}

// User-defined implementation
enum UserEnum: string {
    case B = 'b';

    public static function values(): array {
        return array_map(fn($c) => $c->value, self::cases());
    }
}

$nativeMethod = new ReflectionMethod(NativeEnum::class, 'values');
$userMethod = new ReflectionMethod(UserEnum::class, 'values');

echo "Native is internal: " . ($nativeMethod->isInternal() ? 'yes' : 'no') . "\n";
echo "User is internal: " . ($userMethod->isInternal() ? 'yes' : 'no') . "\n";

?>
--EXPECT--
Native is internal: yes
User is internal: no
