<?php

$dom = new domDocument;
$dom->load('shipping.xml');
if (!$dom->schemaValidate('shipping.xsd')) {
  print "Document is not valid";
} else {
  print "Document is valid";
}

?>