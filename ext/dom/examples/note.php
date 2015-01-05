<?php

$dom = new domDocument;
$dom->load('note.xml');
if (!$dom->validate('note.dtd')) {
  print "Document note.dtd is not valid\n";
} else {
  print "Document note.dtd is valid\n";
}

$dom = new domDocument;
$dom->load('note-invalid.xml');
if (!$dom->validate('note.dtd')) {
  print "Document note-invalid.xml is not valid\n";
} else {
  print "Document note-invalid.xml is valid\n";
}

?>
