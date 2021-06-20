--TEST--
Enum supports static methods
--FILE--
<?php

enum Size {
    case Small;
    case Medium;
    case Large;

    public static function fromLength(int $cm) {
        return match(true) {
            $cm < 50 => static::Small,
            $cm < 100 => static::Medium,
            default => static::Large,
        };
    }
}

var_dump(Size::fromLength(23));
var_dump(Size::fromLength(63));
var_dump(Size::fromLength(123));

?>
--EXPECT--
enum(Size::Small)
enum(Size::Medium)
enum(Size::Large)
