--TEST--
GH-17486 (Incorrect error line numbers reported in Dom\HTMLDocument::createFromString)
--EXTENSIONS--
dom
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
		<div>&#xa;</div>
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
Warning: Dom\HTMLDocument::createFromString(): tokenizer error control-character-reference in Entity, line: 7, column: 9 in %s on line %d

Warning: Dom\HTMLDocument::createFromFile(): tokenizer error control-character-reference in %sgh17486.tmp, line: 7, column: 9 in %s on line %d
