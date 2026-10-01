--TEST--
mb_regex_set_options()
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_regex_set_options') or die("skip\n");
?>
--FILE--
<?php
    mb_regex_set_options( 'x' );
    print mb_ereg_replace(' -', '+', '- - - - -' );

    mb_regex_set_options( '' );
    print mb_ereg_replace(' -', '+', '- - - - -' );
?>
--EXPECTF--
Deprecated: Function mb_regex_set_options() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d

Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
+ + + + +
Deprecated: Function mb_regex_set_options() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
-++++
