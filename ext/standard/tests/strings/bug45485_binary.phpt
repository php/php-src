--TEST--
Bug #45485 (strip_tags and <?XML tag), binary variant
--FILE--
<?php

$s = (binary) <<< EOD
This text is shown <?XML:NAMESPACE PREFIX = ST1 /><b>This Text disappears</b>
EOD;

$s = strip_tags($s);
echo htmlspecialchars($s),"\n";

$s = (binary) <<< EOD
This text is shown <?xml:NAMESPACE PREFIX = ST1 /><b>This Text disappears</b>
EOD;

$s = strip_tags($s);
echo htmlspecialchars($s),"\n";

?>
--EXPECT--
This text is shown This Text disappears
This text is shown This Text disappears
