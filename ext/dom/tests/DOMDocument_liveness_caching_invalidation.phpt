--TEST--
DOMDocument liveness caching invalidation by textContent
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$root = $doc->createElement( 'root' );
$doc->appendChild( $root );
for ( $i = 0; $i < 5; $i++ ) {
    $root->appendChild( $doc->createElement( 'e' ) );
}

$i = 0;

foreach ( $doc->getElementsByTagName( 'e' ) as $node ) {
    if ($i++ == 2) {
        $root->textContent = 'overwrite';
    }
    var_dump($node->tagName);
}
?>
--EXPECTF--
string(1) "e"
string(1) "e"
string(1) "e"
