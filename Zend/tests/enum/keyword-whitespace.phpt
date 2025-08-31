--TEST--
Enum keyword can be followed by arbitrary whitespaces
--FILE--
<?php

enum A {}
enum  B {}
enum	C {}
enum	 E {}
enum
F {}
enum
 G {}

?>
--EXPECT--
