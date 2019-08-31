--TEST--
Bug #78340: Include of stream wrapper not reading whole file
--FILE--
<?php

class lib {
  public static $files= [];

  private $bytes, $pos;

  function stream_open($path, $mode, $options, $opened_path) {
    $this->bytes= self::$files[$path];
    $this->pos= 0;
    $this->ino= crc32($path);
    return true;
  }

  function stream_read($count) {
    $chunk= substr($this->bytes, $this->pos, $count);
    $this->pos+= strlen($chunk);
    return $chunk;
  }

  function stream_eof() {
    return $this->pos >= strlen($this->bytes);
  }

  function stream_close() {
    $this->bytes= null;
  }

  function stream_stat() {
    return [
      'dev'   => 3632233996,
      'size'  => strlen($this->bytes),
      'ino'   => $this->ino
    ];
  }

  function stream_set_option($option, $arg1, $arg2) {
    return false;
  }
}

$fill = str_repeat('.', 8192);
lib::$files['lib://test.php']= '<?php /* '.$fill.' */ function test() { echo "Works!\n"; }';
stream_wrapper_register('lib', lib::class);

include('lib://test.php');
test();

?>
--EXPECT--
Works!
