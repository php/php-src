--TEST--
GH-17486 (Incorrect error line numbers reported in Dom\HTMLDocument::createFromString)
--EXTENSIONS--
dom
--INI--
error_reporting=E_ALL
--CREDITS--
xPaw
--FILE--
<?php

$repeated = str_repeat('a', 50000);

$html = <<<HTML
<!DOCTYPE html>
<html lang="en">
	<body>
		<svg>
			<path d="{$repeated}" />
		</svg>
		<div>&#x0;</div>
	</body>
</html>
HTML;

\Dom\HTMLDocument::createFromString($html);

file_put_contents(__DIR__ . '/gh17486.tmp', $html);
\Dom\HTMLDocument::createFromFile(__DIR__ . '/gh17486.tmp');

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh17486.tmp');
?>
--EXPECTF--
Warning: Dom\HTMLDocument::createFromString(): tokenizer error null-character-reference in Entity, line: 7, column: 9 in %s on line %d

Warning: Dom\HTMLDocument::createFromFile(): tokenizer error null-character-reference in %s line: 7, column: 9 in %s on line %d
