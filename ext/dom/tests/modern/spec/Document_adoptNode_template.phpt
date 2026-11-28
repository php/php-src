--TEST--
Dom\Document::adoptNode() preserves HTML <template> contents (direct, wrapped, nested)
--EXTENSIONS--
dom
--FILE--
<?php

$src = Dom\HTMLDocument::createFromString(
    '<template><p>direct</p></template><div><template><p>inner</p><template><span>deep</span></template></template></div>',
    LIBXML_NOERROR
);
$dst = Dom\HTMLDocument::createEmpty();

$direct = $src->getElementsByTagName('template')[0];
$dst->adoptNode($direct);
echo $dst->saveHtml($direct), "\n";
echo $direct->innerHTML, "\n";

$div = $src->getElementsByTagName('div')[0];
$dst->adoptNode($div);
echo $dst->saveHtml($div), "\n";

unset($src);
echo $dst->saveHtml($direct), "\n";
echo $direct->innerHTML, "\n";
echo $dst->saveHtml($div), "\n";
?>
--EXPECT--
<template><p>direct</p></template>
<p>direct</p>
<div><template><p>inner</p><template><span>deep</span></template></template></div>
<template><p>direct</p></template>
<p>direct</p>
<div><template><p>inner</p><template><span>deep</span></template></template></div>
