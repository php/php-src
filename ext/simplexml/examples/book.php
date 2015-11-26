<?php
$books = simplexml_load_file('book.xml');
//var_dump($books);
$books = $books->book;
foreach ($books as $book) {
	echo "{$book->title} was written by {$book->author}\n";
}
?>
