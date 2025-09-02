--TEST--
GH-12980 (tidynode.props.attribute is missing "Boolean Attributes" and empty attributes)
--EXTENSIONS--
tidy
--FILE--
<?php
$html = '<!DOCTYPE html><html lang="en" boolean empty="" selected="selected"></html>';

$tidy = new tidy();
$tidy->ParseString($html);
echo tidy_get_output($tidy), "\n";

var_dump($tidy->root()->child[1]->attribute);

?>
--EXPECT--
<!DOCTYPE html>
<html lang="en" boolean="" empty="" selected="selected">
<head>
<title></title>
</head>
<body>
</body>
</html>
array(4) {
  ["lang"]=>
  string(2) "en"
  ["boolean"]=>
  string(0) ""
  ["empty"]=>
  string(0) ""
  ["selected"]=>
  string(8) "selected"
}
