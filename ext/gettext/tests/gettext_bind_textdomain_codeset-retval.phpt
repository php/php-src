--TEST--
test if bind_textdomain_codeset() returns correct value
--EXTENSIONS--
gettext
--FILE--
<?php
    var_dump(bind_textdomain_codeset(false,false));

    // bind_textdomain_codeset() always returns false on musl
    // because musl only supports UTF-8. For more information:
    //
    //   * https://github.com/php/doc-en/issues/4311,
    //   * https://github.com/php/php-src/issues/17163
    //
    $result = bind_textdomain_codeset('messages', "UTF-8");
    var_dump($result === false || $result === "UTF-8");

    echo "Done\n";
?>
--EXPECT--
bool(false)
bool(true)
Done
--CREDITS--
Florian Holzhauer fh-pt@fholzhauer.de
PHP Testfest Berlin 2009-05-09
