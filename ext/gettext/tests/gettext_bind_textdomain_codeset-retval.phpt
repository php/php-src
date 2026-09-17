--TEST--
test if bind_textdomain_codeset() returns correct value
--EXTENSIONS--
gettext
--FILE--
<?php
    try {
    	bind_textdomain_codeset(false,false);
    } catch (Throwable $e) {
	    echo $e::class, ': ', $e->getMessage(), "\n";
    }

    try {
    	bind_textdomain_codeset("", "UTF-8");
    } catch (Throwable $e) {
	    echo $e::class, ': ', $e->getMessage(), "\n";
    }

    // bind_textdomain_codeset() always returns false on musl
    // prior to v1.2.6. For more information:
    //
    //   * https://github.com/php/doc-en/issues/4311,
    //   * https://github.com/php/php-src/issues/17163
    //
    // As of v1.2.6, it returns "UTF-8" as well.
    $result = bind_textdomain_codeset('messages', "UTF-8");
    var_dump($result === false || $result === "UTF-8");

    echo "Done\n";
?>
--EXPECT--
ValueError: bind_textdomain_codeset(): Argument #1 ($domain) must not be empty
ValueError: bind_textdomain_codeset(): Argument #1 ($domain) must not be empty
bool(true)
Done
--CREDITS--
Florian Holzhauer fh-pt@fholzhauer.de
PHP Testfest Berlin 2009-05-09
