<?php
$books = simplexml_load_file('book.xml')->book;

foreach ($books as $book) {
	echo "{$book->title} was written by {$book->author}\n";
}
?>
