--TEST--
IntlRelativeDateTimeFormatter API surface
--EXTENSIONS--
intl
--FILE--
<?php

$reflection = new ReflectionClass(IntlRelativeDateTimeFormatter::class);
var_dump($reflection->isFinal());
var_dump($reflection->isInternal());
var_dump($reflection->isInstantiable());
var_dump($reflection->getConstants());

foreach ([
    IntlRelativeDateTimeFormatterStyle::class,
    IntlRelativeDateTimeFormatterCapitalization::class,
    IntlRelativeDateTimeFormatterUnit::class,
] as $class) {
    $enum = new ReflectionEnum($class);
    echo $enum->getName(), "\n";
    var_dump($enum->isInternal(), $enum->isBacked());
    echo implode(', ', array_column($class::cases(), 'name')), "\n";
}

$constructor = $reflection->getConstructor();
foreach ($constructor->getParameters() as $parameter) {
    echo $parameter->getName(), ': ', $parameter->getType(), "\n";
    $default = $parameter->getDefaultValue();
    echo 'default: ', $default instanceof UnitEnum ? $default::class . '::' . $default->name : 'null', "\n";
}

foreach (['format', 'formatNumeric'] as $name) {
    $method = $reflection->getMethod($name);
    echo $name, ': ', $method->getReturnType(), "\n";
    foreach ($method->getParameters() as $parameter) {
        echo $parameter->getName(), ': ', $parameter->getType(), "\n";
    }
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
array(0) {
}
IntlRelativeDateTimeFormatterStyle
bool(true)
bool(false)
Long, Short, Narrow
IntlRelativeDateTimeFormatterCapitalization
bool(true)
bool(false)
None, MiddleOfSentence, BeginningOfSentence, UiListAndMenu, Standalone
IntlRelativeDateTimeFormatterUnit
bool(true)
bool(false)
Year, Quarter, Month, Week, Day, Hour, Minute, Second, Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday
locale: ?string
default: null
style: IntlRelativeDateTimeFormatterStyle
default: IntlRelativeDateTimeFormatterStyle::Long
capitalizationContext: IntlRelativeDateTimeFormatterCapitalization
default: IntlRelativeDateTimeFormatterCapitalization::None
numberFormatter: ?NumberFormatter
default: null
format: string|false
offset: int|float
unit: IntlRelativeDateTimeFormatterUnit
formatNumeric: string|false
offset: int|float
unit: IntlRelativeDateTimeFormatterUnit
