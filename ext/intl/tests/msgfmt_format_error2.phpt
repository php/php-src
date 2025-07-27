--TEST--
MessageFormatter::format() inconsistent types in named argument
--EXTENSIONS--
intl
--INI--
intl.use_exceptions=On
--FILE--
<?php

$fmt = <<<EOD
{foo,number} {foo}
EOD;

$mf = new MessageFormatter('en_US', $fmt);
try {
    var_dump($mf->format(array(7)));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
IntlException: Inconsistent types declared for an argument
