--TEST--
test if bind_textdomain_codeset() returns correct value
--EXTENSIONS--
gettext
--FILE--
<?php
    try {
    	bind_textdomain_codeset(false,false);
    } catch (ValueError $e) {
	    echo $e->getMessage() . PHP_EOL;
    }

    try {
    	bind_textdomain_codeset("", "UTF-8");
    } catch (ValueError $e) {
	    echo $e->getMessage() . PHP_EOL;
    }

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
bind_textdomain_codeset(): Argument #1 ($domain) must not be empty
bind_textdomain_codeset(): Argument #1 ($domain) must not be empty
bool(true)
Done
--CREDITS--
Florian Holzhauer fh-pt@fholzhauer.de
PHP Testfest Berlin 2009-05-09
