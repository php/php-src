--TEST--
MessageFormatter::format() given negative arg key
--EXTENSIONS--
intl
--INI--
intl.use_exceptions=On
--FILE--
<?php

$fmt = <<<EOD
{foo,number,percent}
EOD;

$mf = new MessageFormatter('en_US', $fmt);
try {
    var_dump($mf->format(array("foo" => 7, -1 => "bar")));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
IntlException: Found negative or too large array key
