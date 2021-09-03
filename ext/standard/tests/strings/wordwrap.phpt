--TEST--
wordwrap() function
--FILE--
<?php

var_dump("12345 12345 12345 12345" === wordwrap("12345 12345 12345 12345"));
var_dump("12345 12345\n1234567890\n1234567890" === wordwrap("12345 12345 1234567890 1234567890",12));
var_dump("12345\n12345\n12345\n12345" === wordwrap("12345 12345 12345 12345",0));
var_dump("12345ab12345ab12345ab12345" === wordwrap("12345 12345 12345 12345",0,"ab"));
var_dump("12345 12345ab1234567890ab1234567890" === wordwrap("12345 12345 1234567890 1234567890",12,"ab"));
var_dump("123ab123ab123" === wordwrap("123ab123ab123", 3, "ab"));
var_dump("123ab123ab123" === wordwrap("123ab123ab123", 5, "ab"));
var_dump("123ab 123ab123" === wordwrap("123  123ab123", 3, "ab"));
var_dump("123ab123ab123" === wordwrap("123 123ab123", 5, "ab"));
var_dump("123 123ab123" === wordwrap("123 123 123", 10, "ab"));

var_dump("123ab123ab123" === wordwrap("123ab123ab123", 3, "ab", 1));
var_dump("123ab123ab123" === wordwrap("123ab123ab123", 5, "ab", 1));
var_dump("123ab 12ab3ab123" === wordwrap("123  123ab123", 3, "ab", 1));
var_dump("123 ab123ab123" === wordwrap("123  123ab123", 5, "ab", 1));
var_dump("123  123ab 123" === wordwrap("123  123  123", 8, "ab", 1));
var_dump("123 ab12345 ab123" === wordwrap("123  12345  123", 8, "ab", 1));
var_dump("1ab2ab3ab4" === wordwrap("1234", 1, "ab", 1));

var_dump("12345|12345|67890" === wordwrap("12345 1234567890", 5, "|", 1));

var_dump("123|==1234567890|==123" === wordwrap("123 1234567890 123", 10, "|==", 1));


try {
    wordwrap(chr(0), 0, "");
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
wordwrap(): Argument #3 ($break) cannot be empty
