--TEST--
MessageFormatter::format() invalid type for key not in pattern
--EXTENSIONS--
intl
--INI--
intl.use_exceptions=On
--FILE--
<?php

$fmt = <<<EOD
{foo}
EOD;

$mf = new MessageFormatter('en_US', $fmt);
try {
    var_dump($mf->format(array("foo" => 'bar', 7 => fopen('php://memory', 'r+'))));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
IntlException: No strategy to convert the value given for the argument with key '7' is available
