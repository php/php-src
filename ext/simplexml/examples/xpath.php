<?php
$books = simplexml_load_file('book.xml');

$xpath_result = $books->xpath("/books/book/title");
foreach($xpath_result as $entry ) {
    print "$entry \n";
}

?>
