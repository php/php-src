--TEST--
Return type allowed in child when parent does not have return type
--FILE--
<?php
class Comment {}

class CommentsIterator extends ArrayIterator implements Iterator {
    function current() : Comment {
        return parent::current();
    }
}

$comments = new CommentsIterator([new Comment]);
foreach ($comments as $comment) {
    var_dump($comment);
}
--EXPECTF--
object(Comment)#%d (%d) {
}
