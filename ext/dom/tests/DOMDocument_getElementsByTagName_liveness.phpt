--TEST--
DOMDocument::getElementsByTagName() is live
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML( '<root><e i="1"/><e i="2"/><e i="3"/><e i="4"/><e i="5"/><e i="6"/><e i="7"/><e i="8"/><e i="9"/><e i="10"/><e i="11"/><e i="12"/><e i="13"/><e i="14"/><e i="15"/></root>' );
$root = $doc->documentElement;

$i = 0;

/* Note that the list is live. The explanation for the output is as follows:
   Before the loop we have the following (writing only the attributes):
   1 2 3 4 5 6 7 8 9 10 11 12 13 14 15

   Now the loop starts, the current element is marked with a V. $i == 0:
   V
   1  2  3  4  5  6  7  8  9  10  11  12  13  14  15
   1 gets printed. $i == 0, which is even, so 1 gets removed, which results in:
   V
   2  3  4  5  6  7  8  9  10  11  12  13  14  15
   Note that everything shifted to the left.
   Because the list is live, the current element pointer still refers to the first index, which now corresponds to element with attribute 2.
   Now the foreach body ends, which means we go to the next element, which is now 3 instead of 2.
      V
   2  3  4  5  6  7  8  9  10  11  12  13  14  15
   3 gets printed. $i == 1, which is odd, so nothing happens and we move on to the next element:
         V
   2  3  4  5  6  7  8  9  10  11  12  13  14  15
   4 gets printed. $i == 2, which is even, so 4 gets removed, which results in:
         V
   2  3  5  6  7  8  9  10  11  12  13  14  15
   Note again everything shifted to the left.
   Now the foreach body ends, which means we go to the next element, which is now 6 instead of 5.
            V
   2  3  5  6  7  8  9  10  11  12  13  14  15
   6 gets printed, etc... */
foreach ($doc->getElementsByTagName('e') as $node) {
	print $node->getAttribute('i') . ' ';
	if ($i++ % 2 == 0)
		$root->removeChild($node);
}
print "\n";
?>
--EXPECT--
1 3 4 6 7 9 10 12 13 15
