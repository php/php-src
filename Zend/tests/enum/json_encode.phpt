--TEST--
Enum in json_encode
--FILE--
<?php

enum Foo {
    case Bar;
}

enum IntFoo: int {
    case Bar = 0;
}

enum StringFoo: string {
    case Bar = 'Bar';
}

enum CustomFoo implements JsonSerializable {
    case Bar;

    public function jsonSerialize(): mixed {
        return 'Custom ' . $this->name;
    }
}

function test($value) {
    var_dump(json_encode($value));
    echo json_last_error_msg() . "\n";
    if (json_last_error() !== JSON_ERROR_NONE) {
        echo json_last_error() . ' === '  . JSON_ERROR_NON_BACKED_ENUM . ":\n";
        var_dump(json_last_error() === JSON_ERROR_NON_BACKED_ENUM);
    }

    try {
        var_dump(json_encode($value, JSON_THROW_ON_ERROR));
        echo json_last_error_msg() . "\n";
    } catch (Exception $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }
}

test(Foo::Bar);
test(IntFoo::Bar);
test(StringFoo::Bar);
test(CustomFoo::Bar);

?>
--EXPECT--
bool(false)
Non-backed enums have no default serialization
11 === 11:
bool(true)
JsonException: Non-backed enums have no default serialization
string(1) "0"
No error
string(1) "0"
No error
string(5) ""Bar""
No error
string(5) ""Bar""
No error
string(12) ""Custom Bar""
No error
string(12) ""Custom Bar""
No error
