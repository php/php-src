<?php

$dom = new domDocument;
$dom->load('relaxNG.xml');
if (!$dom->relaxNGValidate('relaxNG.rng')) {
  print "Document is not valid";
} else {
  print "Document is valid";
}

?>