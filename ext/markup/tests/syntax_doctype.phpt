--TEST--
Markup syntax: <!DOCTYPE html> emission rules and non-doctype <! parse errors
--EXTENSIONS--
markup
--FILE--
<?php
// --- doctype emission ---
$x = 'ignored';

// A full document: doctype in a fragment, emitted verbatim before the root element.
echo (<>
    <!DOCTYPE html>
    <html lang="en"><body><p>hi</p></body></html>
</>)->__toString(), PHP_EOL;

// Case-insensitive, and content is literal (no interpolation).
echo (<><!doctype html {$x}><div></div></>)->__toString(), PHP_EOL;

// Legacy doctypes with public/system identifiers pass through too.
echo (<><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"><p>x</p></>)->__toString(), PHP_EOL;

// Position is not validated: emitted where written, like a comment.
echo (<div><!DOCTYPE html><span>s</span></div>)->__toString(), PHP_EOL;

// --- non-doctype <! declarations are a targeted parse error ---
try {
    eval('$v = <svg><![CDATA[ x < y ]]></svg>;');
} catch (ParseError $e) {
    echo $e->getMessage(), PHP_EOL;
}
try {
    eval('$v = <div><!ELEMENT foo></div>;');
} catch (ParseError $e) {
    echo $e->getMessage(), PHP_EOL;
}
// An unterminated doctype must not scan past the end of the file.
try {
    eval('$v = <div><!doctype html');
} catch (ParseError $e) {
    echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
<!DOCTYPE html><html lang="en"><body><p>hi</p></body></html>
<!doctype html {$x}><div></div>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"><p>x</p>
<div><!DOCTYPE html><span>s</span></div>
Unsupported markup declaration; only <!-- --> comments and <!doctype> are supported
Unsupported markup declaration; only <!-- --> comments and <!doctype> are supported
%s
