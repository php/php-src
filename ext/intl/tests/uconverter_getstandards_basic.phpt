--TEST--
Basic UConverter::getStandards() usage
--EXTENSIONS--
intl
--FILE--
<?php
function assertTrue($assertion, $msg) {
    if (!$assertion) var_dump($msg);
}
$standards = UConverter::getStandards();
assertTrue(is_array($standards), '$standards must be an array');
assertTrue(count($standards) > 0, '$standards must not be empty');
assertTrue($standards === array_values($standards), '$standards keys must be numeric');
assertTrue($standards === array_unique($standards), '$standards values must be unique');
assertTrue(array_reduce($standards, function($carry, $item) { return $carry && is_string($item); }, true), '$standards values must be strings');
?>
===DONE===
--EXPECT--
===DONE===
