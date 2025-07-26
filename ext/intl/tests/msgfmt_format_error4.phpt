--TEST--
MessageFormatter::format() invalid UTF-8 for arg key or value
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
    var_dump($mf->format(array("foo" => 7, "\x80" => "bar")));
} catch (Throwable $e) {
    var_dump($e::class === 'IntlException');
    var_dump("Invalid UTF-8 data in argument key: '\x80'" ===  $e->getMessage());
}

try {
    var_dump($mf->format(array("foo" => "\x80")));
} catch (Throwable $e) {
    var_dump($e::class === 'IntlException');
    var_dump("Invalid UTF-8 data in string argument: '\x80'" ===  $e->getMessage());
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
