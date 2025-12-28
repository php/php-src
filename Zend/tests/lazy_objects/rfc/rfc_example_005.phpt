--TEST--
Lazy objects: RFC example 005
--FILE--
<?php

class BlogPost
{
    public function __construct(
        public int $id,
        public string $title,
        public string $content,
    ) {
    }
}

function loadFromDB() {
    return [
        'title' => 'Title',
        'content' => 'Content',
    ];
}

$reflector = new ReflectionClass(BlogPost::class);
// Callable that retrieves the title and content from the database.
$initializer = function ($blogPost) {
    var_dump("initialization");
    $data = loadFromDB();
    return new BlogPost($blogPost->id, $data['title'], $data['content']);
};
$post = $reflector->newLazyProxy($initializer);

// Without this line, the following call to ReflectionProperty::setValue() would trigger initialization.
$reflector->getProperty('id')->skipLazyInitialization($post);
$reflector->getProperty('id')->setValue($post, 123);

// Alternatively, one can use this directly:
$reflector->getProperty('id')->setRawValueWithoutLazyInitialization($post, 123);

var_dump($post);
var_dump($post->id);
var_dump($post->title);
var_dump($post->content);
var_dump($post);

?>
==DONE==
--EXPECTF--
lazy proxy object(BlogPost)#%d (1) {
  ["id"]=>
  int(123)
  ["title"]=>
  uninitialized(string)
  ["content"]=>
  uninitialized(string)
}
int(123)
string(14) "initialization"
string(5) "Title"
string(7) "Content"
lazy proxy object(BlogPost)#%d (1) {
  ["instance"]=>
  object(BlogPost)#%d (3) {
    ["id"]=>
    int(123)
    ["title"]=>
    string(5) "Title"
    ["content"]=>
    string(7) "Content"
  }
}
==DONE==
