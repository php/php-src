--TEST--
Test common ODBC string functionality
--EXTENSIONS--
odbc
--FILE--
<?php

// 1. No, it's not quoted.
// 2. Yes, it should be quoted because of the special character in the middle.
$with_end_curly1 = "foo}bar";
// 1. No, the unescaped special character in the middle breaks what would be quoted.
// 2. Yes, it should be quoted because of the special character in the middle.
//    Note that should_quote doesn't care about if the string is already quoted.
//    That's why you should check if it is quoted first.
$with_end_curly2 = "{foo}bar}";
// 1. Yes, the special characters are escaped, so it's quoted.
// 2. See $with_end_curly2; should_quote doesn't care about if the string is already quoted.
$with_end_curly3 = "{foo}}bar}";
// 1. No, it's not quoted.
// 2. It doesn't need to be quoted because of no s
$with_no_end_curly1 = "foobar";
// 1. Yes, it is quoted and any characters are properly escaped.
// 2. See $with_end_curly2.
$with_no_end_curly2 = "{foobar}";

echo "# Is quoted?\n";
echo "With end curly brace 1: ";
var_dump(odbc_connection_string_is_quoted($with_end_curly1));
echo "With end curly brace 2: ";
var_dump(odbc_connection_string_is_quoted($with_end_curly2));
echo "With end curly brace 3: ";
var_dump(odbc_connection_string_is_quoted($with_end_curly3));
echo "Without end curly brace 1: ";
var_dump(odbc_connection_string_is_quoted($with_no_end_curly1));
echo "Without end curly brace 2: ";
var_dump(odbc_connection_string_is_quoted($with_no_end_curly2));

echo "# Should quote?\n";
echo "With end curly brace 1: ";
var_dump(odbc_connection_string_should_quote($with_end_curly1));
echo "With end curly brace 2: ";
var_dump(odbc_connection_string_should_quote($with_end_curly2));
echo "With end curly brace 3: ";
var_dump(odbc_connection_string_should_quote($with_end_curly3));
echo "Without end curly brace 1: ";
var_dump(odbc_connection_string_should_quote($with_no_end_curly1));
echo "Without end curly brace 2: ";
var_dump(odbc_connection_string_should_quote($with_no_end_curly2));

echo "# Quote?\n";
echo "With end curly brace 1: ";
var_dump(odbc_connection_string_quote($with_end_curly1));
echo "With end curly brace 2: ";
var_dump(odbc_connection_string_quote($with_end_curly2));
echo "With end curly brace 3: ";
var_dump(odbc_connection_string_quote($with_end_curly3));
echo "Without end curly brace 1: ";
var_dump(odbc_connection_string_quote($with_no_end_curly1));
echo "Without end curly brace 2: ";
var_dump(odbc_connection_string_quote($with_no_end_curly2));

?>
--EXPECTF--
# Is quoted?
With end curly brace 1: bool(false)
With end curly brace 2: bool(false)
With end curly brace 3: bool(true)
Without end curly brace 1: bool(false)
Without end curly brace 2: bool(true)
# Should quote?
With end curly brace 1: bool(true)
With end curly brace 2: bool(true)
With end curly brace 3: bool(true)
Without end curly brace 1: bool(false)
Without end curly brace 2: bool(true)
# Quote?
With end curly brace 1: string(10) "{foo}}bar}"
With end curly brace 2: string(13) "{{foo}}bar}}}"
With end curly brace 3: string(15) "{{foo}}}}bar}}}"
Without end curly brace 1: string(8) "{foobar}"
Without end curly brace 2: string(11) "{{foobar}}}"
