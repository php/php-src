--TEST--
Markup syntax: <!DOCTYPE html> is allowed in content position and emitted verbatim (RFC §6)
--EXTENSIONS--
html
--FILE--
<?php
$x = 'ignored';

// A full document: doctype in a fragment, emitted verbatim before the root element.
echo (<>
    <!DOCTYPE html>
    <html lang="en"><body><p>hi</p></body></html>
</>)->__toString(), "\n";

// Case-insensitive, and content is literal (no interpolation).
echo (<><!doctype html {$x}><div></div></>)->__toString(), "\n";

// Legacy doctypes with public/system identifiers pass through too.
echo (<><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"><p>x</p></>)->__toString(), "\n";

// Position is not validated: emitted where written, like a comment.
echo (<div><!DOCTYPE html><span>s</span></div>)->__toString(), "\n";
?>
--EXPECT--
<!DOCTYPE html><html lang="en"><body><p>hi</p></body></html>
<!doctype html {$x}><div></div>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"><p>x</p>
<div><!DOCTYPE html><span>s</span></div>
