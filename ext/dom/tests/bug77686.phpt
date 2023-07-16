--TEST--
Bug #77686 (Removed elements are still returned by getElementById)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;
$doc->loadHTML('<html id="htmlelement"><body id="x">before<div id="y">hello</div>after</body></html>');
$body = $doc->getElementById('x');
$div = $doc->getElementById('y');
var_dump($doc->getElementById('y')->textContent);

// Detached from document, should not find it anymore
$body->removeChild($div);
var_dump($doc->getElementById('y'));

// Added again, should find it
$body->appendChild($div);
var_dump($doc->getElementById('y')->textContent);

// Should find root element without a problem
var_dump($doc->getElementById('htmlelement')->textContent);

// Created element but not yet attached, should not find it before it is added
$new_element = $doc->createElement('p');
$new_element->textContent = 'my new text';
$new_element->setAttribute('id', 'myp');
var_dump($doc->getElementById('myp'));
$body->appendChild($new_element);
var_dump($doc->getElementById('myp')->textContent);

?>
--EXPECT--
string(5) "hello"
NULL
string(5) "hello"
string(16) "beforeafterhello"
NULL
string(11) "my new text"
