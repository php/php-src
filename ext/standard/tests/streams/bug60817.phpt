--TEST--
Bug #60817: stream_get_line() reads from stream even when there is already sufficient data buffered
--FILE--
<?php
class TestStream { //data, empty data, empty data + eof
    private $s = 0;
    function stream_open($path, $mode, $options, &$opened_path) {
            return true;
    }
    function stream_read($count) {
        echo "Read done\n";
        if ($this->s++ == 0)
            return "a\nbb\ncc";

        return "";
    }
    function stream_eof() {
        return $this->s >= 2;
    }

}

stream_wrapper_register("test", "TestStream");

$f = fopen("test://", "r");
while (!feof($f)) {
    $line = stream_get_line($f, 99, "\n");
    var_dump($line);
}
?>
--EXPECT--
Read done
string(1) "a"
string(2) "bb"
Read done
string(2) "cc"
