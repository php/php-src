--TEST--
Bug #37158 (if userspace stream is present, fread() reads in 8192 max, otherwise it works)
--FILE--
<?php

class VariableStream {

   function stream_open($path, $mode, $options, &$opened_path)
   {
       return true;
   }
}

stream_wrapper_register("var", "VariableStream");

$file = dirname(__FILE__) . '/footest.txt';
$x = str_repeat(1, 8192);
$fp = fopen($file, 'w');
for ($i = 0; $i < 5; $i++) {
	fwrite($fp, $x);
}
fclose($fp);

$fp = fopen($file, 'r');
$outsidecontents = fread($fp, 20000);
fclose($fp);
var_dump('size of contents 1 = ' . strlen($outsidecontents));
$outsidecontents = file_get_contents($file);
var_dump('size of contents 2 = ' . strlen($outsidecontents));

unlink($file);

echo "Done\n";
?>
--EXPECT--
string(26) "size of contents 1 = 20000"
string(26) "size of contents 2 = 40960"
Done
