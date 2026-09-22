--TEST--
GHSA-88hq-2827-7pg6: OOB read in convert stream filters when line-break-chars contains NUL
--FILE--
<?php

// Test base64-encode: line-break-chars with embedded NUL must be preserved fully.
// Before the fix, pestrdup() truncated at the NUL, but lbchars_len kept the
// original length, causing a heap-buffer-overflow read during encoding.
$opts = ["line-length" => 4, "line-break-chars" => "\0X"];
$fp = fopen("php://temp", "r+");
fwrite($fp, str_repeat("A", 6));
rewind($fp);
stream_filter_append($fp, "convert.base64-encode", STREAM_FILTER_READ, $opts);
$out = stream_get_contents($fp);
fclose($fp);

// base64("AAAAAA") = "QUFBQUFB" (8 chars)
// With line-length=4: "QUFB" + "\0X" + "QUFB"
// Hex: 5155464200585155 4642
echo bin2hex($out), "\n";

echo "Done\n";

?>
--EXPECT--
51554642005851554642
Done
