--TEST--
Check that "internal_encoding" ini is picked up by mbstring
--EXTENSIONS--
mbstring
--INI--
internal_encoding=iso-8859-1
--FILE--
<?php

var_dump(mb_internal_encoding());
var_dump(mb_strlen("\xc3\xb6"));

ini_set('mbstring.internal_encoding', 'utf-8');

var_dump(mb_internal_encoding());
var_dump(mb_strlen("\xc3\xb6"));

// mbstring.internal_encoding is set, this has no effect
ini_set('internal_encoding', 'iso-8859-2');

var_dump(mb_internal_encoding());
var_dump(mb_strlen("\xc3\xb6"));

// mbstring.internal_encoding is unset, pick up internal_encoding again
ini_set('mbstring.internal_encoding', '');

var_dump(mb_internal_encoding());
var_dump(mb_strlen("\xc3\xb6"));

mb_internal_encoding('utf-8');

var_dump(mb_internal_encoding());
var_dump(mb_strlen("\xc3\xb6"));

// mb_internal_encoding() is set, this has no effect
ini_set('internal_encoding', 'iso-8859-3');

var_dump(mb_internal_encoding());
var_dump(mb_strlen("\xc3\xb6"));

// mbstring.internal_encoding is unset, pick up internal_encoding again
ini_set('mbstring.internal_encoding', '');

var_dump(mb_internal_encoding());
var_dump(mb_strlen("\xc3\xb6"));

?>
--EXPECTF--
string(10) "ISO-8859-1"
int(2)

Deprecated: ini_set(): Use of mbstring.internal_encoding is deprecated in %s on line %d
string(5) "UTF-8"
int(1)
string(5) "UTF-8"
int(1)

Deprecated: ini_set(): Use of mbstring.internal_encoding is deprecated in %s on line %d
string(10) "ISO-8859-2"
int(2)
string(5) "UTF-8"
int(1)
string(5) "UTF-8"
int(1)

Deprecated: ini_set(): Use of mbstring.internal_encoding is deprecated in %s on line %d
string(10) "ISO-8859-3"
int(2)
