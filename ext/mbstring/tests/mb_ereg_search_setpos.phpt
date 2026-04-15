--TEST--
mb_ereg_search_setpos() function
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_regex_encoding")) die("skip mb_regex_encoding() is not defined");
?>
--FILE--
<?php
mb_regex_encoding('iso-8859-1');
$test_str = 'Iñtërnâtiônàlizætiøn'; // Length = 20

var_dump(mb_ereg_search_setpos(50)); // OK
try {
    var_dump(mb_ereg_search_setpos(-1)); // Error
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

mb_ereg_search_init($test_str);

$positions = array( 5, 20, 21, 25, 0, -5, -20, -30);
foreach($positions as $pos) {
    echo("\n* Position: $pos :\n");
    try {
        var_dump(mb_ereg_search_setpos($pos));
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    try {
        var_dump(mb_ereg_search_getpos());
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}
?>
--EXPECTF--
Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_setpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_ereg_search_setpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_ereg_search_setpos(): Argument #1 ($offset) is out of range

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

* Position: 5 :

Deprecated: Function mb_ereg_search_setpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(5)

* Position: 20 :

Deprecated: Function mb_ereg_search_setpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(20)

* Position: 21 :

Deprecated: Function mb_ereg_search_setpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_ereg_search_setpos(): Argument #1 ($offset) is out of range

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(20)

* Position: 25 :

Deprecated: Function mb_ereg_search_setpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_ereg_search_setpos(): Argument #1 ($offset) is out of range

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(20)

* Position: 0 :

Deprecated: Function mb_ereg_search_setpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(0)

* Position: -5 :

Deprecated: Function mb_ereg_search_setpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(15)

* Position: -20 :

Deprecated: Function mb_ereg_search_setpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
bool(true)

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(0)

* Position: -30 :

Deprecated: Function mb_ereg_search_setpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
mb_ereg_search_setpos(): Argument #1 ($offset) is out of range

Deprecated: Function mb_ereg_search_getpos() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
int(0)
