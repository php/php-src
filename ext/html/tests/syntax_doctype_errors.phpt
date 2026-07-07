--TEST--
Markup syntax: non-doctype <! declarations are a targeted parse error
--EXTENSIONS--
html
--FILE--
<?php
try {
    eval('$v = <svg><![CDATA[ x < y ]]></svg>;');
} catch (ParseError $e) {
    echo $e->getMessage(), "\n";
}
try {
    eval('$v = <div><!ELEMENT foo></div>;');
} catch (ParseError $e) {
    echo $e->getMessage(), "\n";
}
// An unterminated doctype must not scan past the end of the file.
try {
    eval('$v = <div><!doctype html');
} catch (ParseError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Unsupported markup declaration; only <!-- --> comments and <!doctype> are supported
Unsupported markup declaration; only <!-- --> comments and <!doctype> are supported
%s
