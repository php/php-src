--TEST--
Directory Streams
--FILE--
<?php
class test {
    public $idx = 0;
    public $context;

    function dir_opendir($path, $options) {
        print "Opening\n";
        $this->idx = 0;

        return true;
    }

    function dir_readdir() {
        $sample = array('first','second','third','fourth');

        if ($this->idx >= count($sample)) return false;
                                    else  return $sample[$this->idx++];
    }

    function dir_rewinddir() {
        $this->idx = 0;

        return true;
    }

    function dir_closedir() {
        print "Closing up!\n";

        return true;
    }
}

stream_wrapper_register('test', 'test');

var_dump(scandir('test://example.com/path/to/test'));
?>
--EXPECT--
Opening
Closing up!
array(4) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "fourth"
  [2]=>
  string(6) "second"
  [3]=>
  string(5) "third"
}
