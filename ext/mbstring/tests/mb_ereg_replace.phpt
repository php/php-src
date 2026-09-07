--TEST--
mb_ereg_replace()
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
    mb_regex_set_options( '' );
    print mb_ereg_replace( ' ', '-', 'a b c d e' )."\n";
    print mb_ereg_replace( '([a-z]+)','[\\1]', 'abc def ghi' );
?>
--EXPECTF--
Deprecated: Function mb_regex_set_options() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
a-b-c-d-e

Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
[abc] [def] [ghi]
