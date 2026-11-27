--TEST--
date_get_last_errors() return false
--CREDITS--
"Anna Filina" <afilina@phpquebec.org>
#PHPTestFest2009 2009-05-02
--INI--
date.timezone=UTC
--FILE--
<?php
var_dump(date_get_last_errors()); // no date was parsed, so no errors

// Constructing "now" must reset errors left over from a failed parse.
foreach ([
    'new DateTime()' => fn() => new DateTime(),
    'new DateTime("now")' => fn() => new DateTime('now'),
    'new DateTimeImmutable("now")' => fn() => new DateTimeImmutable('now'),
    'date_create()' => fn() => date_create(),
    'date_create("now")' => fn() => date_create('now'),
] as $name => $create) {
    date_create('garbage');
    var_dump(date_get_last_errors()['error_count'] > 0);
    $create();
    echo $name, ': ';
    var_dump(date_get_last_errors());
}

// A failed parse after a successful "now" must still record errors.
new DateTime();
date_create('garbage');
var_dump(date_get_last_errors()['error_count'] > 0);
?>
--EXPECT--
bool(false)
bool(true)
new DateTime(): bool(false)
bool(true)
new DateTime("now"): bool(false)
bool(true)
new DateTimeImmutable("now"): bool(false)
bool(true)
date_create(): bool(false)
bool(true)
date_create("now"): bool(false)
bool(true)
