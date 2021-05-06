--TEST--
stream filter - reading
--FILE--
<?php
echo "-TEST\n";
class strtoupper_filter extends php_user_filter {
    function filter($in, $out, &$consumed, $closing): int
    {
        $output = 0;
        while ($bucket = stream_bucket_make_writeable($in)) {
            $bucket->data = strtoupper($bucket->data);
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
            $output = 1;
        }
        if ($closing) {
            $bucket = stream_bucket_new($this->stream, "\n===close===\n");
            stream_bucket_append($out, $bucket);
            $output = 1;
        }
        return $output ? PSFS_PASS_ON : PSFS_FEED_ME;
    }
}
stream_filter_register("strtoupper", "strtoupper_filter")
   or die("Failed to register filter");

if ($f = fopen(__FILE__, "rb")) {
    stream_filter_append($f, "strtoupper");
    while (!feof($f)) {
        echo fread($f, 8192);
    }
    fclose($f);
}
echo "Done\n";
?>
--EXPECTF--
%sTEST
<?PHP
ECHO "-TEST\N";
CLASS STRTOUPPER_FILTER EXTENDS PHP_USER_FILTER {
    FUNCTION FILTER($IN, $OUT, &$CONSUMED, $CLOSING): INT
    {
        $OUTPUT = 0;
        WHILE ($BUCKET = STREAM_BUCKET_MAKE_WRITEABLE($IN)) {
            $BUCKET->DATA = STRTOUPPER($BUCKET->DATA);
            $CONSUMED += $BUCKET->DATALEN;
            STREAM_BUCKET_APPEND($OUT, $BUCKET);
            $OUTPUT = 1;
        }
        IF ($CLOSING) {
            $BUCKET = STREAM_BUCKET_NEW($THIS->STREAM, "\N===CLOSE===\N");
            STREAM_BUCKET_APPEND($OUT, $BUCKET);
            $OUTPUT = 1;
        }
        RETURN $OUTPUT ? PSFS_PASS_ON : PSFS_FEED_ME;
    }
}
STREAM_FILTER_REGISTER("STRTOUPPER", "STRTOUPPER_FILTER")
   OR DIE("FAILED TO REGISTER FILTER");

IF ($F = FOPEN(__FILE__, "RB")) {
    STREAM_FILTER_APPEND($F, "STRTOUPPER");
    WHILE (!FEOF($F)) {
        ECHO FREAD($F, 8192);
    }
    FCLOSE($F);
}
ECHO "DONE\N";
?>

===close===
Done
