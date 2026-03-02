--TEST--
GH-17481 (UTF-8 corruption in \Dom\HTMLDocument)
--EXTENSIONS--
dom
--FILE--
<?php

$inputs = [
    [str_repeat('–', 4096), false],
    [str_repeat('😏', 4096), false],
    [str_repeat('–', 4096), true],
    [str_repeat('😏', 4096), true],
    [str_repeat('–', 1358), false],
    [str_repeat('–', 1359), false],
];

foreach ($inputs as [$input, $endTag]) {
    $Data = "<!DOCTYPE HTML><html>$input";
    if ($endTag) {
        $Data .= '</html>';
    }
    $Document = \Dom\HTMLDocument::createFromString($Data, 0, 'UTF-8');
    var_dump($Document->body->textContent === $input);
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
