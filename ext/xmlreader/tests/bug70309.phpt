--TEST--
XMLReader: Bug #70309 XmlReader read generates extra output
--SKIPIF--
<?php if (substr(PHP_OS, 0, 3) != 'WIN') die('skip.. only for Windows'); ?>
<?php if (!extension_loaded("xmlreader")) print "skip"; ?>
<?php if (getenv("SKIP_SLOW_TESTS")) die("skip slow test"); ?>
--FILE--
<?php

/* From the bug report an extra message comes from libxml2 debug output left unwrapped. */

$doc = new \XMLReader();

$doc->xml('<?xml version="1.0" encoding="UTF-8"?>
<chapter xmlns="http://docbook.org/ns/docbook" version="5.0">
<title>Test Chapter</title>
<para>
This is a paragraph in the test chapter. It is unremarkable in
every regard. This is a paragraph in the test chapter. It is
unremarkable in every regard. This is a paragraph in the test
chapter. It is unremarkable in every regard.
</para>
<paar>
<emphasis role="bold">This</emphasis> paragraph contains
<emphasis>some <emphasis>emphasized</emphasis> text</emphasis>
1and a <superscript>super</superscript>script
and a <subscript>sub</subscript>script.
</para>
<para>
This is a paragraph in the test chapter. It is unremarkable in
every regard. This is a paragraph in the test chapter. It is
unremarkable in every regard. This is a paragraph in the test
chapter. It is unremarkable in every regard.
</para>
</chapter>');

$doc->setRelaxNGSchema('http://docs.oasis-open.org/docbook/rng/5.0/docbook.rng');

while (@$doc->read() !== false);
?>
===DONE===
--EXPECT--
===DONE===
