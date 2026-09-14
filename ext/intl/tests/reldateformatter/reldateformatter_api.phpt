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
var_dump(count($reflection->getConstants()));

foreach ([
    'STYLE_LONG',
    'STYLE_SHORT',
    'STYLE_NARROW',
    'CAPITALIZATION_NONE',
    'CAPITALIZATION_FOR_MIDDLE_OF_SENTENCE',
    'CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE',
    'CAPITALIZATION_FOR_UI_LIST_OR_MENU',
    'CAPITALIZATION_FOR_STANDALONE',
    'UNIT_YEAR',
    'UNIT_QUARTER',
    'UNIT_MONTH',
    'UNIT_WEEK',
    'UNIT_DAY',
    'UNIT_HOUR',
    'UNIT_MINUTE',
    'UNIT_SECOND',
    'UNIT_SUNDAY',
    'UNIT_MONDAY',
    'UNIT_TUESDAY',
    'UNIT_WEDNESDAY',
    'UNIT_THURSDAY',
    'UNIT_FRIDAY',
    'UNIT_SATURDAY',
] as $constant) {
    var_dump($reflection->getReflectionConstant($constant)->getType()->getName());
}

$constructor = $reflection->getConstructor();
foreach ($constructor->getParameters() as $parameter) {
    echo $parameter->getName(), ': ', $parameter->getType(), "\n";
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
int(23)
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
string(3) "int"
locale: ?string
style: int
capitalizationContext: int
numberFormatter: ?NumberFormatter
