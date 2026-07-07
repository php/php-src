--TEST--
Html\Htmlable: a class implementing neither toHtml() nor __toString() only owes toHtml()
--EXTENSIONS--
html
--FILE--
<?php
// __toString is satisfied by the injected default, so the one remaining
// abstract requirement is toHtml() - the error must name only it.
class Nope implements Html\Htmlable {}
?>
--EXPECTF--
Fatal error: Class Nope contains 1 abstract method and must therefore be declared abstract or implement the remaining method (Html\Htmlable::toHtml) in %s on line %d
