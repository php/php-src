--TEST--
MessageFormatter with invalid locale
--EXTENSIONS--
intl
--FILE--
<?php

$fmt = '{0,whatever} would not work!';

try {
    $mf = new MessageFormatter('root', $fmt);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$mf = MessageFormatter::create('root', $fmt);
var_dump($mf);
var_dump(intl_get_error_message());

$mf = msgfmt_create('root', $fmt);
var_dump($mf);
var_dump(intl_get_error_message());

?>
--EXPECT--
IntlException: msgfmt_create: message formatter creation failed: U_ILLEGAL_ARGUMENT_ERROR
NULL
string(74) "msgfmt_create: message formatter creation failed: U_ILLEGAL_ARGUMENT_ERROR"
NULL
string(74) "msgfmt_create: message formatter creation failed: U_ILLEGAL_ARGUMENT_ERROR"
