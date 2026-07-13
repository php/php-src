--TEST--
Markup\Html: a class implementing neither toHtml() nor __toString() only owes toHtml()
--EXTENSIONS--
markup
--FILE--
<?php
// __toString is satisfied by the injected default, so the one remaining
// abstract requirement is toHtml() - the error must name only it.
class Nope implements Markup\Html {}
?>
--EXPECTF--
Fatal error: Class Nope contains 1 abstract method and must therefore be declared abstract or implement the remaining method (Markup\Html::toHtml) in %s on line %d
