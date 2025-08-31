--TEST--
htmlentities() / htmlspecialchars() ENT_SUBSTITUTE EUC-JP
--FILE--
<?php
$tests = array(
    "\x8F\xA1\xFF", //2 sub as 2nd is potentially valid as lead
    "\x8F\xA1", //2 sub, as 2nd is potentially valid as lead
    "\x8F", //1 sub
    "\x8F\xA0", //1 sub, A0 is not valid as sole/first byte
    "\x8F\xA1\x21", //2 sub, no consume last
    "\x8F\x21", //1 sub, no consume last
    "\x8E\xAE", //valid
    "\x8E", //1 sub
    "\x8E\x21", //1 sub, no consume last
    "\xB2\xFF", //1 sub
    "\xB2", //1 sub
    "\xB2\x21", //1 sub, no consume last
    "\xA0", //1 sub
);

foreach ($tests as $test) {
    $a = htmlentities($test, ENT_QUOTES | ENT_SUBSTITUTE, "EUC-JP");
    var_dump($a, bin2hex($a));
    $a = htmlspecialchars($test, ENT_QUOTES | ENT_SUBSTITUTE, "EUC-JP");
    var_dump($a, bin2hex($a));
    echo "\n";
}
?>
--EXPECTF--

Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(16) "&#xFFFD;&#xFFFD;"
string(32) "262378464646443b262378464646443b"
string(16) "&#xFFFD;&#xFFFD;"
string(32) "262378464646443b262378464646443b"


Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(16) "&#xFFFD;&#xFFFD;"
string(32) "262378464646443b262378464646443b"
string(16) "&#xFFFD;&#xFFFD;"
string(32) "262378464646443b262378464646443b"


Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(8) "&#xFFFD;"
string(16) "262378464646443b"
string(8) "&#xFFFD;"
string(16) "262378464646443b"


Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(8) "&#xFFFD;"
string(16) "262378464646443b"
string(8) "&#xFFFD;"
string(16) "262378464646443b"


Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(17) "&#xFFFD;&#xFFFD;!"
string(34) "262378464646443b262378464646443b21"
string(17) "&#xFFFD;&#xFFFD;!"
string(34) "262378464646443b262378464646443b21"


Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(9) "&#xFFFD;!"
string(18) "262378464646443b21"
string(9) "&#xFFFD;!"
string(18) "262378464646443b21"


Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(2) "��"
string(4) "8eae"
string(2) "��"
string(4) "8eae"


Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(8) "&#xFFFD;"
string(16) "262378464646443b"
string(8) "&#xFFFD;"
string(16) "262378464646443b"


Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(9) "&#xFFFD;!"
string(18) "262378464646443b21"
string(9) "&#xFFFD;!"
string(18) "262378464646443b21"


Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(8) "&#xFFFD;"
string(16) "262378464646443b"
string(8) "&#xFFFD;"
string(16) "262378464646443b"


Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(8) "&#xFFFD;"
string(16) "262378464646443b"
string(8) "&#xFFFD;"
string(16) "262378464646443b"


Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(9) "&#xFFFD;!"
string(18) "262378464646443b21"
string(9) "&#xFFFD;!"
string(18) "262378464646443b21"


Notice: htmlentities(): Only basic entities substitution is supported for multi-byte encodings other than UTF-8; functionality is equivalent to htmlspecialchars in %s line %d
string(8) "&#xFFFD;"
string(16) "262378464646443b"
string(8) "&#xFFFD;"
string(16) "262378464646443b"
