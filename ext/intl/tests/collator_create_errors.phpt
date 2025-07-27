--TEST--
Collator creation errors
--EXTENSIONS--
intl
--FILE--
<?php

$locales = [
    'uk-ua_CALIFORNIA@currency=;currency=GRN',
    str_repeat('a', 160),
];

foreach ($locales as $locale) {
    try {
        $c = new Collator($locale);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }

    $c = Collator::create($locale);
    var_dump($c);
    var_dump(intl_get_error_message());

    $c = collator_create($locale);
    var_dump($c);
    var_dump(intl_get_error_message());
}

?>
--EXPECT--
IntlException: Constructor failed
NULL
string(70) "collator_create: unable to open ICU collator: U_ILLEGAL_ARGUMENT_ERROR"
NULL
string(70) "collator_create: unable to open ICU collator: U_ILLEGAL_ARGUMENT_ERROR"
IntlException: Constructor failed
NULL
string(89) "Locale string too long, should be no longer than 156 characters: U_ILLEGAL_ARGUMENT_ERROR"
NULL
string(89) "Locale string too long, should be no longer than 156 characters: U_ILLEGAL_ARGUMENT_ERROR"