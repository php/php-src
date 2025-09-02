<?php
function createElement($doc, $name, ?string $value = null) {
    $element = $doc->createElement($name);
    if ($value) $element->textContent = $value;
    return $element;
}

function createElementNS($doc, $ns, $name, ?string $value = null) {
    $element = $doc->createElementNS($ns, $name);
    if ($value) $element->textContent = $value;
    return $element;
}
