<?php
$indent = 5; /* Number of spaces to indent per level */

$reader = new XMLReader();
$reader->open('relaxNG.xml');
/*
Example setting relaxNG using string:
$reader->setRelaxNGSchemaSource(file_get_contents('relaxNG.rng'));
*/
if ($reader->setRelaxNGSchema('relaxNG.rng')) {
  while ($reader->read()) {
    /* Print node name indenting it based on depth and $indent var */
    print str_repeat(" ", $reader->depth * $indent).$reader->name."\n";
  }
}

print "\n";

if (! $reader->isValid()) {
  print "Document is not valid\n";
} else {
  print "Document is valid\n";
}

?>