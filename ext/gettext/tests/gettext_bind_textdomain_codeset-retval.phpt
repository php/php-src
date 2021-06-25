--TEST--
test if bind_textdomain_codeset() returns correct value
--EXTENSIONS--
gettext
--FILE--
<?php
    var_dump(bind_textdomain_codeset(false,false));
    var_dump(bind_textdomain_codeset('messages', "UTF-8"));

    echo "Done\n";
?>
--EXPECTF--
Deprecated: Implicit bool to string coercion is deprecated in %s on line %d

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
bool(false)
string(5) "UTF-8"
Done
