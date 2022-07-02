--TEST--
compress.zlib:// wrapper with compression level
--EXTENSIONS--
zlib
--SKIPIF--
<?php in_array('compress.zlib', stream_get_wrappers()) || print 'skip No zlib wrapper';
--FILE--
<?php declare(strict_types=1);

$filename = tempnam(sys_get_temp_dir(), "php-zlib-test-");
$thisfile = file_get_contents(__FILE__);

function write_at_level(int $level) {
  global $filename, $thisfile;

  $ctx = stream_context_create(['zlib' => ['level' => $level] ]);
  $fp = fopen("compress.zlib://$filename", 'w', false, $ctx);
  for ($i = 0; $i < 10; ++$i) {
    fwrite($fp, $thisfile);
  }
  fclose($fp);
  $size = filesize($filename);
  unlink($filename);
  return $size;
}

$size1 = write_at_level(1);
$size9 = write_at_level(9);

var_dump(10 * strlen($thisfile));
var_dump($size1);
var_dump($size9);
var_dump($size9 < $size1);
?>
--EXPECTF--
int(%d)
int(%d)
int(%d)
bool(true)
