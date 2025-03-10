--TEST--
`never` parameter types - BackedEnum uses never, backed enums have parameter types
--FILE--
<?php

function enumMethod(string $class, string $name) {
    echo "$class::$name():\n";
    $method = new ReflectionMethod($class, $name);
    if ($method->hasPrototype()) {
        $proto = $method->getPrototype();
        echo "Prototype: " . $proto->class . "::" . $proto->name . "\n";
    }
    $param = $method->getParameters()[0];
    echo $param . "\n";

    $type = $param->getType();

    if (!($type instanceof ReflectionNamedType)) {
        throw new Exception("Should be named...");
    }
    echo "Name: " . $type->getName() . "\n";
    echo "isBuiltin: " . $type->isBuiltIn() . "\n";
    echo "allowsNull: " . (int)$type->allowsNull() . "\n";
}

enum MyBoolean: int {
    case FALSE = 0;
    case TRUE = 1;
}
enum CardSuit: string {
    case HEARTS = 'Hearts';
    case DIAMONDS = 'Diamonds';
    case CLUBS = 'Clubs';
    case SPADES = 'Spades';
}

enumMethod(BackedEnum::class, "from");
echo "\n";
enumMethod(BackedEnum::class, "tryFrom");
echo "\n\n";

enumMethod(MyBoolean::class, "from");
echo "\n";
enumMethod(MyBoolean::class, "tryFrom");
echo "\n\n";

enumMethod(CardSuit::class, "from");
echo "\n";
enumMethod(CardSuit::class, "tryFrom");

?>
--EXPECT--
BackedEnum::from():
Parameter #0 [ <required> never $value ]
Name: never
isBuiltin: 1
allowsNull: 0

BackedEnum::tryFrom():
Parameter #0 [ <required> never $value ]
Name: never
isBuiltin: 1
allowsNull: 0


MyBoolean::from():
Prototype: BackedEnum::from
Parameter #0 [ <required> int $value ]
Name: int
isBuiltin: 1
allowsNull: 0

MyBoolean::tryFrom():
Prototype: BackedEnum::tryFrom
Parameter #0 [ <required> int $value ]
Name: int
isBuiltin: 1
allowsNull: 0


CardSuit::from():
Prototype: BackedEnum::from
Parameter #0 [ <required> string $value ]
Name: string
isBuiltin: 1
allowsNull: 0

CardSuit::tryFrom():
Prototype: BackedEnum::tryFrom
Parameter #0 [ <required> string $value ]
Name: string
isBuiltin: 1
allowsNull: 0
