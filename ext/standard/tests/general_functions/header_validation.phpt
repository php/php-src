--TEST--
header() validations
--CGI--
--FILE--
<?php
// "headers already sent" condition triggered by output would take precedence,
// so some magic is needed to capture actual validation errors.
$errors = [];
$errorHandler = function($errno, $errstr) use (&$errors) {
	$errors[] = $errstr;
	return true;
};
set_error_handler($errorHandler, E_WARNING);

// These should be accepted
header(""); // ignored
header("HTTP/", false, 200);
header("HTTP/1.0", true, 200);
header("HTTP/1.1 200", true, 200);
header("HTTP/1.1 201 Reason phrase", true, 201);
header("Empty-value:");
header("!#$%&'*+-.^_`|~: These are all the valid header name characters beside ASCII alphanumeric");
header("OWS:Optional white space is optional");
header("OWS: \t \t Optional whitespace is of any length", false);
header("Trailing-whitespace: Trailing spaces, tabs and CRLF are trimmed by PHP (CRLF is not legal, we're fault-tolerant with it)\r\n \t");
$erroneous_warnings = $errors;
$errors = [];

// All of these should trigger warnings
header("HTTP/");
header("HTTP/1", true, 200);
header("HTTP/1 200", true, 200);
header("HTTP/0.5", true, 200);
header("HTTP/1.1 099");
header("HTTP/1.1 500Missing space before reason-phrase");
header("HTTP/1.1\t400 Only a single space is valid here");
header("HTTP/1.1  400 Only a single space is valid here");
header("HTTP/1.1 400\tOnly a single space is valid here");
header("http/1.1 500 Protocol can't be lowercase");
header("HTTP/1.1 500 NUL\x00bytes not allowed");
header("HTTP/1.1 500 Newlines\rnot allowed");
header("HTTP/1.1 500 Newlines\nnot allowed");
header(":empty name");
header("Invalid;name:");
header("Header : can't have space in name");
header(" Header: Leading whitespace is not allowed");
header_remove("Cannot-contain-colon:");
set_error_handler(null);

if (!empty($erroneous_warnings)) {
	echo "*** Unexpected warnings that shouldn't have occurred ***\n";
	foreach ($erroneous_warnings as $message) {
		echo "Warning: {$message}\n";
	}
	echo "*** End of unexpected warnings ***\n";
}
foreach ($errors as $message) {
	echo "Warning: {$message}\n";
}
echo "Done\n";
?>
--EXPECTHEADERS--
Status: 201 Reason phrase
Empty-value:
!#$%&'*+-.^_`|~: These are all the valid header name characters beside ASCII alphanumeric
OWS:Optional white space is optional
OWS: 	 	 Optional whitespace is of any length
Trailing-whitespace: Trailing spaces, tabs and CRLF are trimmed by PHP (CRLF is not legal, we're fault-tolerant with it)
--EXPECT--
Warning: Invalid HTTP status line
Warning: Invalid HTTP status line
Warning: Invalid HTTP status line
Warning: Invalid HTTP status line
Warning: Invalid HTTP status line
Warning: Invalid HTTP status line
Warning: Invalid HTTP status line
Warning: Invalid HTTP status line
Warning: Invalid HTTP status line
Warning: Invalid character detected in header
Warning: Header may not contain NUL bytes
Warning: Header may not contain more than a single header, new line detected
Warning: Header may not contain more than a single header, new line detected
Warning: Header name may not be empty
Warning: Invalid character detected in header
Warning: Invalid character detected in header
Warning: Invalid character detected in header
Warning: Header to delete may not contain colon.
Done
