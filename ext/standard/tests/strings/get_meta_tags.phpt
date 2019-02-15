--TEST--
get_meta_tags() tests
--FILE--
<?php

$filename = dirname(__FILE__)."/get_meta_tags.html";

$data = <<<DATA
<meta name="author" content="name">
<meta name="keywords" content="php documentation">
<meta name="DESCRIPTION" content="a php manual">
<meta name="geo.position" content="49.33;-86.59">
</head> <!-- parsing stops here -->
DATA;

$data1 = <<<DATA
<html>
    <head>
        <meta name="author" content="name">
        <meta name="keywords" content="php documentation">
        <meta name="DESCRIPTION" content="a php manual">
        <meta name="geo.position" content="49.33;-86.59">
    </head>
    <body>
        <meta name="author" content="name1">
        <meta name="keywords" content="php documentation1">
        <meta name="DESCRIPTION" content="a php manual1">
        <meta name="geo.position" content="49.33;-86.591">
    </body>
</html>
DATA;

$data2 = <<<DATA
<meta name="author" content="name"
<meta name="keywords" content="php documentation">
DATA;

$data3 = <<<DATA
<meta <meta name="keywords" content="php documentation">
DATA;

$data4 = <<<DATA
<meta name="author" content="name"
<meta name="keywords" content="php documentation"
DATA;

$array = array($data, $data1, $data2, $data3, $data4, "", "<>", "<meta<<<<<");

foreach ($array as $html) {
	file_put_contents($filename, $html);
	var_dump(get_meta_tags($filename));
}

@unlink($filename);

echo "Done\n";
?>
--EXPECT--
array(4) {
  ["author"]=>
  string(4) "name"
  ["keywords"]=>
  string(17) "php documentation"
  ["description"]=>
  string(12) "a php manual"
  ["geo_position"]=>
  string(12) "49.33;-86.59"
}
array(4) {
  ["author"]=>
  string(4) "name"
  ["keywords"]=>
  string(17) "php documentation"
  ["description"]=>
  string(12) "a php manual"
  ["geo_position"]=>
  string(12) "49.33;-86.59"
}
array(1) {
  ["keywords"]=>
  string(17) "php documentation"
}
array(1) {
  ["keywords"]=>
  string(17) "php documentation"
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
Done
