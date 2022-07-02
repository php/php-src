--TEST--
Test imap_8bit() function : basic functionality
--EXTENSIONS--
imap
--FILE--
<?php
echo "*** Testing imap_8bit() : basic functionality ***\n";

var_dump(imap_8bit("String with CRLF at end \r\n"));
//NB this appears to be a bug in cclient; a space at end of string should be encoded as =20
var_dump(imap_8bit("String with space at end "));
var_dump(imap_8bit("String with tabs \t\t in middle"));
var_dump(imap_8bit("String with tab at end \t"));
var_dump(imap_8bit("\x00\x01\x02\x03\x04\xfe\xff\x0a\x0d"));

?>
--EXPECT--
*** Testing imap_8bit() : basic functionality ***
string(28) "String with CRLF at end=20
"
string(25) "String with space at end "
string(33) "String with tabs =09=09 in middle"
string(26) "String with tab at end =09"
string(27) "=00=01=02=03=04=FE=FF=0A=0D"
