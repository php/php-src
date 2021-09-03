--TEST--
Bug #60455: stream_get_line and 2 lines, one possibly empty
--FILE--
<?php
class TestStream {
    private $lines = array();
    private $s = 0;
    private $eofth = 3;
    function stream_open($path, $mode, $options, &$opened_path) {
            $this->lines[] = "a\n";
            $this->lines[] = ($path == "test://nonempty2nd" ? "b\n" : "\n");
            if ($path == "test://eofafter2nd")
                $this->eofth = 2;
            return true;
    }
    function stream_read($count) {
        if (key_exists($this->s++, $this->lines))
            return $this->lines[$this->s - 1];

        return "";
    }
    function stream_eof() {
        return $this->s >= $this->eofth;
    }

}

stream_wrapper_register("test", "TestStream");

$f = fopen("test://nonempty2nd", "r");
while (!feof($f)) {
    $line = stream_get_line($f, 99, "\n");
    var_dump($line);
}
$f = fopen("test://", "r");
while (!feof($f)) {
    $line = stream_get_line($f, 99, "\n");
    var_dump($line);
}
$f = fopen("test://eofafter2nd", "r");
while (!feof($f)) {
    $line = stream_get_line($f, 99, "\n");
    var_dump($line);
}
?>
--EXPECT--
string(1) "a"
string(1) "b"
bool(false)
string(1) "a"
string(0) ""
bool(false)
string(1) "a"
string(0) ""
