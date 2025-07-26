--TEST--
MessageFormatter::format() invalid date/time argument
--INI--
date.timezone=Atlantic/Azores
intl.use_exceptions=On
--EXTENSIONS--
intl
--FILE--
<?php

$fmt = <<<EOD
{foo,date}
EOD;

$mf = new MessageFormatter('en_US', $fmt);
try {
    var_dump($mf->format(array("foo" => new stdclass())));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
IntlException: The argument for key 'foo' cannot be used as a date or time
