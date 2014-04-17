--TEST--
Return type where none previously existed

--FILE--
<?php

class Comment {
}

class CommentsIterator extends ArrayIterator implements Iterator {
    function current() : Comment {
        return parent::current();
    }
}


$comments = new CommentsIterator([
    new Comment
]);

foreach ($comments as $comment) {
    var_dump($comment);
}
--EXPECTF--
object(Comment)#%d (%d) {
}

