--TEST--
MessageFormatter::format() invalid UTF-8 for arg key or value
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$fmt = <<<EOD
{foo}
EOD;

$mf = new MessageFormatter('en_US', $fmt);
var_dump($mf->format(array("foo" => 7, "\x80" => "bar")));

var_dump($mf->format(array("foo" => "\x80")));

var_dump($mf->format(array("foo" => new class {
    function __toString(): string {
        return str_repeat("\x80", random_int(1, 1));
    }
})));
?>
--EXPECTF--
Warning: MessageFormatter::format(): Invalid UTF-8 data in argument key: '€' in %s on line %d
bool(false)

Warning: MessageFormatter::format(): Invalid UTF-8 data in string argument: '€' in %s on line %d
bool(false)

Warning: MessageFormatter::format(): Invalid UTF-8 data in string argument: '€' in %s on line %d
bool(false)
