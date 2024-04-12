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
    var_dump(bind_textdomain_codeset('messages', "UTF-8"));

    echo "Done\n";
?>
--EXPECT--
bind_textdomain_codeset(): Argument #1 ($domain) cannot be empty
bind_textdomain_codeset(): Argument #1 ($domain) cannot be empty
string(5) "UTF-8"
Done
--CREDITS--
Florian Holzhauer fh-pt@fholzhauer.de
PHP Testfest Berlin 2009-05-09
