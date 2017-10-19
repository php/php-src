--TEST--
setrawcookie() cookie name negative tests
--DESCRIPTION--
--INI--
date.timezone=UTC
--FILE--
<?php
setrawcookie('=');
setrawcookie(',');
setrawcookie(';');
setrawcookie(' ');
setrawcookie("\t");
setrawcookie("\r");
setrawcookie("\n");
setrawcookie("\013");
setrawcookie("\014");
?>
--EXPECTHEADERS--

--EXPECTF--
Warning: Cookie names cannot contain any of the following '=,; \t\r\n\013\014' in %s on line %d

Warning: Cookie names cannot contain any of the following '=,; \t\r\n\013\014' in %s on line %d

Warning: Cookie names cannot contain any of the following '=,; \t\r\n\013\014' in %s on line %d

Warning: Cookie names cannot contain any of the following '=,; \t\r\n\013\014' in %s on line %d

Warning: Cookie names cannot contain any of the following '=,; \t\r\n\013\014' in %s on line %d

Warning: Cookie names cannot contain any of the following '=,; \t\r\n\013\014' in %s on line %d

Warning: Cookie names cannot contain any of the following '=,; \t\r\n\013\014' in %s on line %d

Warning: Cookie names cannot contain any of the following '=,; \t\r\n\013\014' in %s on line %d

Warning: Cookie names cannot contain any of the following '=,; \t\r\n\013\014' in %s on line %d

--CREDITS--
Bill Condo bill@billcondo.com
Matthew Radcliffe mradcliffe@kosada.com
Peter Barney peter@peterbarney.com
Michael Cordingley michael.cordingley@gmail.com
Taylor Howard samueltaylorhoward@gmail.com
# TestFest Columbus PHP UG 2017-10-11