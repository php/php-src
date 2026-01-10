--TEST--
numfmt_parse_currency() reference handling
--EXTENSIONS--
intl
--FILE--
<?php
class Test {
    public int $prop = 1;
}
$test = new Test;
$fmt = numfmt_create( 'de_DE', NumberFormatter::CURRENCY );
$num = "1\xc2\xa0$";
try {
    numfmt_parse_currency($fmt, $num, $test->prop);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Cannot assign string to reference held by property Test::$prop of type int
